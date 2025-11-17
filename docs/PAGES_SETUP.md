# GitHub Pages Setup Instructions

Copyright (C) 2025, Shyamal Suhana Chandra

## Manual Setup Required

GitHub Pages needs to be enabled manually in the repository settings before the workflow can deploy.

### Steps to Enable GitHub Pages:

1. Go to your repository on GitHub:
   https://github.com/Sapana-Micro-Software/progressive-learning-chess-engine

2. Navigate to **Settings** → **Pages**

3. Under **Source**, select:
   - **Source**: `GitHub Actions`
   - (Do NOT select a branch - use GitHub Actions instead)

4. Click **Save**

5. The GitHub Actions workflow will automatically build and deploy your site.

### Alternative: Enable via GitHub CLI

If you have the necessary permissions, you can enable Pages via CLI:

```bash
gh api repos/Sapana-Micro-Software/progressive-learning-chess-engine/pages \
  -X POST \
  -f source[type]=gh_actions \
  -f source[branch]=main
```

### After Enabling

Once Pages is enabled:
- The workflow will run automatically on every push to `main`
- Your site will be available at:
  `https://sapana-micro-software.github.io/progressive-learning-chess-engine/`
- Build logs will be visible in the **Actions** tab

### Troubleshooting

If you see "Get Pages site failed":
1. Verify Pages is enabled in Settings → Pages
2. Check that the source is set to "GitHub Actions" (not a branch)
3. Ensure the workflow file is in `.github/workflows/pages.yml`
4. Check the Actions tab for build errors
