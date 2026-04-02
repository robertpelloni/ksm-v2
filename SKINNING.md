# K-Shoot MANIA v2 Skinning Guide

Welcome to the K-Shoot MANIA v2 skinning guide! K-Shoot MANIA v2 uses a new UI layout engine called **NocoUI**. This allows for much more flexible and dynamic layouts compared to v1.

## NocoUI Overview
NocoUI is a JSON-based layout engine developed specifically for this project (and other OpenSiv3D projects). UI layouts are defined in `.noco` files, which are simply JSON files that describe a hierarchy of visual elements.

You can find the default `.noco` files in the `kshootmania/App/ui/scene/` directory.

### Basic Structure
A `.noco` file contains a single root canvas, which can contain multiple child elements (nodes).

```json
{
  "type": "Canvas",
  "children": [
    {
      "type": "Texture",
      "textureAsset": "bg_main",
      "position": { "x": 0, "y": 0 }
    },
    {
      "type": "Text",
      "text": "Hello World",
      "fontAsset": "system_font",
      "color": "#FFFFFF"
    }
  ]
}
```

## Customizing Layouts

### Modifying .noco files
You can open any `.noco` file in a standard text editor.

*   **Changing Positions/Sizes:** You can adjust the `x`, `y`, `width`, and `height` properties of any element.
*   **Changing Colors:** Colors are usually defined as hex strings (e.g., `"#FF0000"` for red) or RGBA arrays.
*   **Hiding Elements:** You can often hide an element by removing it from the JSON array or setting its `visible` property to `false` (if supported by the specific node type).

### Dynamic Parameters
In K-Shoot MANIA v2, the C++ code communicates with the NocoUI layout by setting "parameters". For example, the Song Select screen passes the current song title, artist, and BPM to specific text nodes in the layout.

If you are modifying a `.noco` file, **do not change the `name` or `tag` properties** of elements that display dynamic data (like scores or titles), as the C++ code relies on these names/tags to find the element and update its content.

## Customizing Assets (Images & Sounds)

Assets (textures, fonts, and audio) are managed via an internal asset management system. Currently, many assets are hardcoded to specific paths in the `App/imgs/` or `App/se/` directories.

To change an image, simply replace the corresponding `.png` or `.jpg` file in the `App/imgs/` directory with your custom image, keeping the same filename.

### Important Directories
*   `App/imgs/`: UI textures, background images, and gameplay sprites.
*   `App/se/`: Sound effects (e.g., menu cursors, judgment sounds).
*   `App/ui/scene/`: The `.noco` layout files.

## Creating a Custom Skin Package
*(Note: Full skin package support via a centralized `skin.ini` or folder selection is currently in development. For now, skinning requires overwriting the default files in the `App/` directory).*

We recommend keeping a backup of the original `App/` folder before making modifications.

## Supported Node Types in NocoUI
Here are some common node types you will see in the `.noco` files:
*   `Texture`: Draws an image.
*   `Text`: Draws text.
*   `Rect`: Draws a solid or bordered rectangle.
*   `Canvas`: A container for other nodes, useful for grouping and relative positioning.
