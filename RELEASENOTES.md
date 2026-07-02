# 3.0.0

### BREAKING CHANGES

- The format of the dialog_gain field parsed from the ISOBMFF init segment data changed from u16 to u8

### Bug fixes

- Fixed an issue causing incompatibility with non-init segments containing multiple boxes
- Made `init->meta->grpl->prsl->labl` boxes optional
- Made `init->meta->grpl->prsl->elng` boxes optional

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
