#!/usr/bin/env node

import { GoogleGenAI } from "@google/genai";

const instructions = {
    short: "instruction: keep the ans short and concise withing 3 to 5 lines if possible, if needed write more. just give me the reponse directly and do not add any extra text\n",
    plainFormat: "instruction: i am on terminal which can not render markdown. so i want you to write the response in plain text. and format or beautify using plain text.\n",
}

if (!process.env.GEMINI_API_KEY) {
    console.error("No env variable named \"GEMINI_API_KEY\"");
    process.exit(1);
}

const ai = new GoogleGenAI({ apiKey: process.env.GEMINI_API_KEY });
const fallbackModel = "gemini-3-flash-preview";
const mainModel = "gemini-3.1-flash-lite-preview";
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
    let userPrompt = args.filter(arg => arg !== "-i").join(" ");

    if (!userPrompt && !pipedInput) {
        console.error("Usage: gemini [-i] [prompt]");
        process.exit(1);
    }

    if (!args.includes("-l")) { // ask for long form response
        userPrompt = instructions.short + userPrompt;
    } else {
        userPrompt = userPrompt.replace("-l", "");
    }
    if (!args.includes("-m")) { // ask for markdown response
        userPrompt = instructions.plainFormat + userPrompt;
    } else {
        userPrompt = userPrompt.replace("-m", "");
    }

    const finalPrompt = pipedInput
        ? `CONTEXT DATA:\n\`\`\`\n${pipedInput}\n\`\`\`\n\nUSER QUESTION:\n\n ${userPrompt}`
        : userPrompt;

    makeCall(finalPrompt);
}

async function makeCall(finalPrompt) {

    try {
        const result = await ai.models.generateContent({
            model: checkingFallback ? fallbackModel : mainModel,
            contents: finalPrompt,
        })

        console.log(result.text);
    } catch (error) {
        if (!checkingFallback) {
            console.warn("[WARNING]\tMain modle failed. Checking fallback.......");
            checkingFallback = true;
            main(fullContent);
        } else {
            console.error("Error:", error.message);
        }
    }
}

main();
