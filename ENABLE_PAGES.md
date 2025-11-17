# ⚠️ IMPORTANT: Enable GitHub Pages

Copyright (C) 2025, Shyamal Suhana Chandra

## GitHub Pages Must Be Enabled Manually

The GitHub API does not support enabling Pages with "GitHub Actions" as the source. You must enable it manually through the GitHub web interface.

## Quick Setup (2 Steps)

### Step 1: Open Pages Settings
Click this link: **https://github.com/Sapana-Micro-Software/progressive-learning-chess-engine/settings/pages**

### Step 2: Configure Source
1. Under **"Source"**, select: **"GitHub Actions"**
   - ⚠️ **NOT** "Deploy from a branch"
   - ⚠️ **NOT** "main" branch
   - ✅ **Select "GitHub Actions"**

2. Click **"Save"**

## What Happens Next

1. The workflow will automatically run
2. Your site will be built and deployed
3. Available at: **https://sapana-micro-software.github.io/progressive-learning-chess-engine/**

## Verification

After enabling, check:
- **Actions** tab: Workflow should run successfully
- **Settings → Pages**: Should show "GitHub Actions" as source
- **Site URL**: Should be accessible after first successful deployment

## Troubleshooting

If you see "Get Pages site failed":
- ✅ Pages is not enabled yet → Follow steps above
- ✅ Source is set to "Deploy from a branch" → Change to "GitHub Actions"
- ✅ Workflow file exists → Check `.github/workflows/pages.yml`

## Current Status

- ✅ Workflow file: Ready
- ✅ Website files: Ready in `docs/dist/`
- ✅ TypeScript: Compiles successfully
- ✅ PDFs: Compiled and ready
- ⚠️ Pages: **Needs manual enablement** (see steps above)
