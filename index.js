#!/usr/bin/env node

import { GoogleGenAI } from "@google/genai";

const instructions = {
    short: "instruction: keep the ans short and concise within 3 to 5 lines if possible, if needed write more. just give me the response directly and do not add any extra text\n",
    plainFormat: "instruction: i am on terminal which can not render markdown. so i want you to write the response in plain text. and format or beautify using plain text.\n",
}

if (!process.env.GEMINI_API_KEY) {
    console.error("No env variable named \"GEMINI_API_KEY\"");
    process.exit(1);
}

const ai = new GoogleGenAI({ apiKey: process.env.GEMINI_API_KEY });
const mainModel = "gemini-3.1-flash-lite";
const fallbackModel = "gemini-3.1-flash-lite-preview";
let checkingFallback = false;

async function readStdin() {
    if (process.stdin.isTTY) {
        return "";
    }

    let data = "";
    for await (const chunk of process.stdin) {
        data += chunk;
    }
    return data;
}

async function main() {
    const pipedInput = await readStdin();

    const args = process.argv.slice(2);

    // Safely extract flags
    const hasLongFlag = args.includes("-l");
    const hasMarkdownFlag = args.includes("-m");

    // Filter out all known flags before joining the user prompt
    const promptArgs = args.filter(arg => !['-i', '-l', '-m'].includes(arg));
    const userPrompt = promptArgs.join(" ");

    if (!userPrompt && !pipedInput) {
        console.error("Usage: gemini [-i] [-l] [-m] [prompt]");
        process.exit(1);
    }

    // Prepend instructions based on the flags
    let activeInstructions = "";
    if (!hasLongFlag) { // If no -l flag, ask for short
        activeInstructions += instructions.short;
    }
    if (!hasMarkdownFlag) { // If no -m flag, ask for plain text
        activeInstructions += instructions.plainFormat;
    }

    const combinedPrompt = activeInstructions + userPrompt;

    const finalPrompt = pipedInput
        ? `CONTEXT DATA:\n\`\`\`\n${pipedInput}\n\`\`\`\n\nUSER QUESTION:\n\n ${combinedPrompt}`
        : combinedPrompt;

    makeCall(finalPrompt);
}

async function makeCall(finalPrompt) {
    try {
        const result = await ai.models.generateContent({
            model: checkingFallback ? fallbackModel : mainModel,
            contents: finalPrompt,
        })

        // Better for terminal piping than console.log
        process.stdout.write(result.text + '\n');

    } catch (error) {
        if (!checkingFallback) {
            console.warn("[WARNING]\tMain model failed. Checking fallback.......");
            console.error("[ERROR]\t" + error + "\n\n");
            checkingFallback = true;

            // Fix: Call makeCall again with the preserved prompt, NOT main()
            await makeCall(finalPrompt);
        } else {
            console.error("Error:", error.message);
            process.exit(1); // Exit with error code so piped commands know it failed
        }
    }
}

main();
