# GitHub Pages Setup Instructions

Copyright (C) 2025, Shyamal Suhana Chandra

## ✅ GitHub Pages is Now Enabled!

GitHub Pages has been enabled for this repository. The site will be automatically built and deployed via GitHub Actions.

### Website URL

Your site will be available at:
**https://sapana-micro-software.github.io/progressive-learning-chess-engine/**

### Automatic Deployment

The GitHub Actions workflow (`.github/workflows/pages.yml`) will:
1. Install Node.js dependencies
2. Compile TypeScript to JavaScript
3. Compile LaTeX documents to PDFs (if LaTeX is available)
4. Build the website
5. Deploy to GitHub Pages

### Manual Configuration (if needed)

If you need to change the Pages configuration manually:

1. Go to: https://github.com/Sapana-Micro-Software/progressive-learning-chess-engine/settings/pages

2. Under **Source**, ensure:
   - **Source**: `GitHub Actions`
   - (NOT "Deploy from a branch")

3. Click **Save**

### Troubleshooting

If the workflow fails:
1. Check the **Actions** tab for build errors
2. Verify Pages is set to "GitHub Actions" in Settings → Pages
3. Ensure the workflow file exists at `.github/workflows/pages.yml`
4. Check that `docs/dist/` contains the built files

### Build Status

You can monitor the build status in the **Actions** tab:
https://github.com/Sapana-Micro-Software/progressive-learning-chess-engine/actions
