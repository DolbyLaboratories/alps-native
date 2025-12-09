# 2.0.0

This release includes the following changes since the 1.0.0

### Features

- Improved ISOBMFF signaling - alps_presentation structure provides more information parsed from ISOBMFF
- New API for Presentation changed callback context fetching `alps_get_presentations_changed_callback_context`

### Bug Fixes

- Fixed errors in docs generation

### BREAKING CHANGES

- alps_presentation structure has changed: `presentation_id` renamed to `id`, `language` has been renamed to `extended_language`, `label` has changed to `labels` and is now an array of `alps_presentation_label`

# 1.0.0

First public release

### Features
- Selection of active presentation from AC-4 bitstream
- Processing ISOBMFF segment to force selected presentation decoding
- Fetching presentations list available in the stream
- Fetching active presentation ID
- Presentations list change detection - callback when detected