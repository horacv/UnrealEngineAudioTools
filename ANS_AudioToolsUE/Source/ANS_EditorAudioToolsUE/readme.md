# ANS_AudioToolsUE

Editor plugin for managing audio assets through Blueprint utilities.

## Features

Provides an editor-only Blueprint function library (`Editor Audio Tools UE`) to:

- Audit assets using Unreal's Asset Audit tool
- Query assets by class via Asset Registry (with optional subclass filtering)
- Identify unreferenced assets based on dependency options
- Filter assets by metadata tags or tag/value pairs
- Bulk add/remove metadata tags and auto-save changes

## Usage

Access functions through the `UEditorAudioToolsFunctionLibrary` Blueprint library or by right-clicking assets and navigating to **Scripted Asset Actions → Audio Tools**.

### Common Workflows

**Audit all Sound Waves:**
1. Create array with `SoundWave` class → `Get All Assets Of Classes`
2. Pass result to `Audit Assets`

**Find unreferenced assets with specific metadata:**
1. Gather assets (by class, selection, or collection)
2. `Filter Assets By Metadata Tag And Value` with target tag/value
3. `Get All Unreferenced Assets In Collection` with desired dependency options

**Bulk-tag selected assets:**
1. Get asset objects from selection → `Add Metadata Entries To Asset Objects` with tag/value pairs

### Note:

- Editor-only (not available in packaged builds)

## Credits

Created by Horacio Valdivieso - Above Noise Studios