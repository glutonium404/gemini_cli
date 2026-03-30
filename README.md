# Prerequisite

- node
- npm
- npx
- `GEMINI_API_KEY` environment variable

```sh
GEMINI_API_KEY='<your api key>'
```

# Convertion to executable

```sh
npm i

npx esbuild index.js --bundle --platform=node --outfile=dist/bundle.js

npx @yao-pkg/pkg dist/bundle.js --target node20-linux-x64 --output gemini-app
```
