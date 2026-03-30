#!/usr/bin/env node

import { GoogleGenerativeAI } from "@google/generative-ai";

const instruction = "keep the ans short and concise withing 3 to 5 lines if possible, if needed write more. do not format the text. just use plain text. just ans the question, do not add extra text, do as told and keep it at that only. Here is my question: \n\n";

if (!process.env.GEMINI_API_KEY) {
    console.error("No env variable named \"GEMINI_API_KEY\"");
    process.exit(1);
}

const genAI = new GoogleGenerativeAI(process.env.GEMINI_API_KEY);

const model = genAI.getGenerativeModel({
    model: "gemini-3.1-flash-lite-preview"
});

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

    const hasNoInstructionFlag = args.includes("-i");

    const userPrompt = args.filter(arg => arg !== "-i").join(" ");

    if (!userPrompt && !pipedInput) {
        console.error("Usage: gemini [-i] [prompt]");
        process.exit(1);
    }

    const finalPrompt = pipedInput
        ? `CONTEXT DATA:\n\`\`\`\n${pipedInput}\n\`\`\`\n\nUSER QUESTION: ${userPrompt}`
        : userPrompt;

    try {
        const fullContent = hasNoInstructionFlag ? finalPrompt : instruction + finalPrompt;

        const result = await model.generateContent(fullContent);
        console.log(result.response.text());
    } catch (error) {
        console.error("Error:", error.message);
    }
}

main();
