#!/usr/bin/env node

import { GoogleGenerativeAI } from "@google/generative-ai";

const instruction = "keep the ans short and concise withing 3 to 5 lines. do not format the text. just use plain text. just ans the question, do not add extra text, do as told and keep it at that only. Here is my question: \n\n";

const question = process.argv.slice(2).join(" ");

if (!question) {
    console.log("Usage: gemini <your question>");
    process.exit(1);
}

if (!process.env.GEMINI_API_KEY) {
    console.error("No env variable named \"GEMINI_API_KEY\"");
    process.exit(1);
}

const genAI = new GoogleGenerativeAI(process.env.GEMINI_API_KEY);

const model = genAI.getGenerativeModel({
    model: "gemini-3.1-flash-lite-preview"
});

async function ask() {
    try {
        const result = await model.generateContent(instruction + question);
        console.log(result.response.text());
    } catch (error) {
        console.error("Error:", error.message);
    }
}

ask();
