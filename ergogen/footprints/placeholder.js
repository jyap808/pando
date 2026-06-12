// Copyright (c) 2026 Julian Yap
//
// SPDX-License-Identifier: MIT
//
// To view a copy of this license, visit https://opensource.org/license/mit/
//
// Author: @jyap808
//
// Ergogen placeholder footprint
// Centered on origin. Swap out for a real footprint in KiCad later.
//
// Parameters:
//   width        - footprint body width  (default: 10)
//   height       - footprint body height (default: 10)
//   cy_margin    - courtyard margin beyond body on all sides (default: 0.5)
//   label        - silkscreen value text, e.g. 'USB-C' or '0603 100nF' (default: 'PLACEHOLDER')
//   description  - freeform description stored in footprint metadata (default: '')
//   back         - place on back copper/fab/courtyard layers instead of front (default: false)
//   show_cross   - draw a centre cross on Dwgs.User (default: true)

module.exports = {
  params: {
    designator:  'PH',
    width:       { type: 'number',  value: 10 },
    height:      { type: 'number',  value: 10 },
    cy_margin:   { type: 'number',  value: 0.5 },
    label:       { type: 'string',  value: 'PLACEHOLDER' },
    description: { type: 'string',  value: '' },
    back:        { type: 'boolean', value: false },
    show_cross:  { type: 'boolean', value: true },
  },

  body: p => {
    const w  = p.width;
    const h  = p.height;
    const m  = p.cy_margin;

    const bx = w / 2;
    const by = h / 2;
    const cx = bx + m;
    const cy = by + m;

    const arm = Math.max(0.5, Math.min(w, h) * 0.2);

    // Layer prefix: F or B
    const s = p.back ? 'B' : 'F';

    const descr = p.description
      ? p.description
      : `Ergogen placeholder (${p.label}) - replace with real footprint in KiCad`;

    const cross = p.show_cross ? `
    (fp_line (start ${-arm} 0) (end ${arm} 0) (layer "Dwgs.User") (width 0.1))
    (fp_line (start 0 ${-arm}) (end 0 ${arm}) (layer "Dwgs.User") (width 0.1))` : '';

    return `
  (footprint "placeholder"
    (layer "${s}.Cu")
    ${p.at}
    (descr "${descr}")
    (tags "placeholder")

    (fp_text reference "${p.ref}" (at 0 ${-(by + m + 1)}) (layer "${s}.SilkS") ${p.ref_hide}
      (effects (font (size 1 1) (thickness 0.15)))
    )
    (fp_text value "${p.label}" (at 0 ${by + m + 1}) (layer "${s}.Fab")
      (effects (font (size 1 1) (thickness 0.15)))
    )

    (fp_rect
      (start ${-bx} ${-by}) (end ${bx} ${by})
      (layer "${s}.SilkS") (width 0.12) (fill none)
    )

    (fp_rect
      (start ${-cx} ${-cy}) (end ${cx} ${cy})
      (layer "${s}.CrtYd") (width 0.05) (fill none)
    )

    (fp_rect
      (start ${-bx} ${-by}) (end ${bx} ${by})
      (layer "${s}.Fab") (width 0.1) (fill none)
    )
    ${cross}
  )`;
  },
};
