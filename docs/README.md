# Progressive Learning Chess Engine - Documentation

Copyright (C) 2025, Shyamal Suhana Chandra

This directory contains the documentation and GitHub Pages website for the Progressive Learning Chess Engine.

## Contents

- **paper.tex** / **paper.pdf**: Complete technical paper describing the architecture, algorithms, and results
- **reference_manual.tex** / **reference_manual.pdf**: Comprehensive API reference documentation
- **presentation.tex** / **presentation.pdf**: Beamer presentation slides
- **public/**: Source files for GitHub Pages website
- **src/**: TypeScript source files
- **dist/**: Compiled website files (deployed to GitHub Pages)

## Building the Website

### Prerequisites

- Node.js 20+
- npm
- LaTeX (for PDF compilation)

### Build Steps

1. Install dependencies:
```bash
npm install
```

2. Compile TypeScript:
```bash
npm run build
```

3. Compile PDFs (if LaTeX is installed):
```bash
npm run compile-pdfs
```

4. Copy PDFs to dist:
```bash
cp paper.pdf reference_manual.pdf presentation.pdf dist/
```

## GitHub Pages

The website is automatically deployed to GitHub Pages via GitHub Actions when changes are pushed to the main branch.

The website includes:
- Animated background canvas with particle system
- Interactive network visualization
- Architecture diagrams
- Modern responsive design
- Links to all documentation PDFs

## Local Development

To run the website locally:

```bash
npm run serve
```

Then open http://localhost:8080 in your browser.

## Copyright

Copyright (C) 2025, Shyamal Suhana Chandra. All rights reserved.
