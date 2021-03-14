from uuid import uuid4
import math


heading = """<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg
   xmlns:dc="http://purl.org/dc/elements/1.1/"
   xmlns:cc="http://creativecommons.org/ns#"
   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
   xmlns:svg="http://www.w3.org/2000/svg"
   xmlns="http://www.w3.org/2000/svg"
   xmlns:sodipodi="http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
   xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
   width="8.5in"
   height="5.5in"
   viewBox="0 0 215.9 139.7"
   version="1.1"
   id="svg8"
   inkscape:version="1.0.2 (e86c870, 2021-01-15)"
   sodipodi:docname="faceplate.svg">
  <defs
     id="defs2">
    <rect
       x="10.953466"
       y="6.3681341"
       width="54.692398"
       height="23.592944"
       id="rect889" />
  </defs>
  <sodipodi:namedview
     id="base"
     pagecolor="#8b0000"
     bordercolor="#666666"
     borderopacity="1.0"
     inkscape:pageopacity="0"
     inkscape:pageshadow="2"
     inkscape:zoom="1.4"
     inkscape:cx="406.71406"
     inkscape:cy="249.49464"
     inkscape:document-units="mm"
     inkscape:current-layer="layer1"
     inkscape:document-rotation="0"
     showgrid="false"
     inkscape:window-width="1872"
     inkscape:window-height="1136"
     inkscape:window-x="1968"
     inkscape:window-y="27"
     inkscape:window-maximized="1"
     units="in"
     inkscape:snap-global="true" />
  <metadata
     id="metadata5">
    <rdf:RDF>
      <cc:Work
         rdf:about="">
        <dc:format>image/svg+xml</dc:format>
        <dc:type
           rdf:resource="http://purl.org/dc/dcmitype/StillImage" />
        <dc:title />
      </cc:Work>
    </rdf:RDF>
  </metadata>
  <g
     inkscape:label="Layer 1"
     inkscape:groupmode="layer"
     id="layer1">
    <text
       xml:space="preserve"
       id="text887"
       style="font-style:normal;font-weight:normal;font-size:10.5833px;line-height:1.25;font-family:sans-serif;white-space:pre;shape-inside:url(#rect889);fill:#000000;fill-opacity:1;stroke:none;stroke-width:1;stroke-miterlimit:4;stroke-dasharray:none;"
       transform="translate(-3.2211028,-0.48201538)"><tspan
         x="10.953125"
         y="15.730927"><tspan
           style="fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:1;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1">zsynth</tspan></tspan></text>
"""

closing = """
  </g>
</svg>
"""

def random_id():
    return str(uuid4())


def text(x, y, text, font_size, stroke_width):
    return f"""
        <text
           xml:space="preserve"
           style="font-style:normal;font-weight:normal;font-size:{font_size}px;line-height:1.25;font-family:sans-serif;fill:#000000;fill-opacity:1;stroke:none;stroke-width:{stroke_width}"
           x="{x}"
           y="{y}"
           id="{random_id()}"><tspan
             sodipodi:role="line"
             id="tspan893"
             x="{x}"
             y="{y}"
             style="fill:#000000;fill-opacity:1;stroke:none;stroke-width:0.0918171;stroke-opacity:1">{text}</tspan></text>
    """


def circle(x, y, r, stroke_width=0.3):
    return f"""
        <circle
           style="fill:none;fill-opacity:1;stroke:#000000;stroke-width:{stroke_width};stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1"
           id="{random_id()}"
           cx="{x}"
           cy="{y}"
           r="{r}" />
    """


def rect(x, y, width, height, ry=0):
    return f"""
        <rect
           style="fill:none;fill-rule:evenodd;stroke:#000000;stroke-width:0.643356;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1"
           id="{random_id()}"
           width="{width}"
           height="{height}"
           x="{x}"
           y="{y}"
           ry="{ry}" />
    """

TEXT_STYLE_LABEL = {
    "font_size": 3.67,
    "stroke_width": 1,
}
TEXT_STYLE_GROUP = {
    "font_size": 5,
    "stroke_width": 1,
}

LOGO_SPACE = 10
CANVAS_PADDING = 5
CANVAS_PADDING_TOP = 8

KNOB_RAD = 5
KNOB_TEXT_SPACING = 1
KNOB_TEXT_HEIGHT = 3
KNOB_BOX_PADDING = 6

KNOB_BOX_WIDTH = KNOB_RAD * 2 + KNOB_BOX_PADDING * 2
KNOB_BOX_HEIGHT = KNOB_RAD * 2 + KNOB_BOX_PADDING * 2 + KNOB_TEXT_HEIGHT

KNOB_GROUP_PADDING = 0
KNOB_GROUP_ROUND = 4.5

CANVAS_WIDTH = 8.5 * 2.54 * 10
CANVAS_HEIGHT = 11 / 2 * 2.54 * 10


def knob_box_render(label, x, y):
    yield text(
        x + KNOB_BOX_PADDING,
        y + KNOB_RAD * 2 + KNOB_BOX_PADDING + KNOB_TEXT_SPACING + KNOB_TEXT_HEIGHT,
        label,
        **TEXT_STYLE_LABEL,
    )
    yield circle(
        x + KNOB_RAD + KNOB_BOX_PADDING,
        y + KNOB_RAD + KNOB_BOX_PADDING,
        KNOB_RAD,
    )


class KnobGroup:
    def __init__(self, label, knob_labels, nwidth):
        self.label = label
        self.knob_labels = knob_labels
        self.nwidth = nwidth
        self.nheight = math.ceil(len(knob_labels) / nwidth)

    @property
    def width(self):
        return self.nwidth * KNOB_BOX_WIDTH + 2 * KNOB_GROUP_PADDING

    @property
    def height(self):
        return self.nheight * KNOB_BOX_HEIGHT + 2 * KNOB_GROUP_PADDING

    def render(self, x, y):
        yield text(x + KNOB_GROUP_ROUND + 0.8, y + TEXT_STYLE_GROUP["font_size"] * 0.3, self.label, **TEXT_STYLE_GROUP)
        yield rect(x, y, width=self.width, height=self.height, ry=KNOB_GROUP_ROUND)
        for i, label in enumerate(self.knob_labels):
            if not label:
                continue
            xx = x + (i % self.nwidth) * KNOB_BOX_WIDTH + KNOB_GROUP_PADDING
            yy = y + (i // self.nwidth) * KNOB_BOX_HEIGHT + KNOB_GROUP_PADDING
            yield from knob_box_render(label, xx, yy)


def main():
    with open("out.svg", "w") as fp:
        gen1 = KnobGroup(
            "Gen1",
            knob_labels=["wave", "coarse", "fine", "amp"],
            nwidth=2,
        )
        gen2 = KnobGroup(
            "Gen2",
            knob_labels=["wave", "coarse", "amp"],
            nwidth=3,
        )
        gen3 = KnobGroup(
            "Gen3",
            knob_labels=["wave", "coarse", "amp"],
            nwidth=3,
        )
        lfo = KnobGroup(
            "LFO",
            knob_labels=["wave", "freq", "param", "send", None, "fm send"],
            nwidth=2,
        )
        output = KnobGroup(
            "Main",
            knob_labels=["effect", "amount", "amp"],
            nwidth=1,
        )
        am_env = KnobGroup(
            "AM Env",
            knob_labels=["attack", "decay", "sustain", "release"],
            nwidth=2,
        )
        fm_env = KnobGroup(
            "FM Env",
            knob_labels=["attack", "decay", "sustain", "release"],
            nwidth=2,
        )
        filt = KnobGroup(
            "Filter",
            knob_labels=["cutoff", "param"],
            nwidth=1,
        )
        assert gen2.width == gen3.width

        row1_group_padding = (CANVAS_WIDTH - 2 * CANVAS_PADDING - gen1.width - gen2.width - lfo.width - output.width) / 3
        col1_group_padding = (CANVAS_HEIGHT - CANVAS_PADDING - CANVAS_PADDING_TOP - LOGO_SPACE - gen1.height - am_env.height) / 2

        fp.write(heading)

        # col 1
        x = CANVAS_PADDING
        y = LOGO_SPACE + CANVAS_PADDING_TOP + col1_group_padding

        fp.writelines(gen1.render(x, y))

        y += gen1.height + col1_group_padding

        fp.writelines(am_env.render(x, y))

        # row 2
        x += am_env.width + row1_group_padding
        row2_start_y = y

        fp.writelines(fm_env.render(x, y))

        x += fm_env.width + row1_group_padding

        fp.writelines(filt.render(x, y))

        # row 1
        x = CANVAS_PADDING + gen1.width + row1_group_padding
        y = CANVAS_PADDING_TOP

        gen2_y = y
        fp.writelines(gen2.render(x, y))

        gen3_padding = (row2_start_y - (gen2_y + gen2.height) - gen3.height) / 2

        y += gen3_padding + gen2.height

        fp.writelines(gen3.render(x, y))

        x += gen2.width + row1_group_padding

        y = CANVAS_PADDING_TOP
        fp.writelines(lfo.render(x, y))
        fp.write(rect(
            x, y + lfo.height - KNOB_BOX_HEIGHT, KNOB_BOX_WIDTH, KNOB_BOX_HEIGHT,
        ))
        fp.write(rect(
            x + KNOB_BOX_WIDTH, y + lfo.height - KNOB_BOX_HEIGHT, KNOB_BOX_WIDTH, KNOB_BOX_HEIGHT,
        ))
        fp.write(rect(
            x, y, KNOB_BOX_WIDTH, lfo.height - KNOB_BOX_HEIGHT,
        ))

        x += lfo.width + row1_group_padding

        fp.writelines(output.render(x, y))

        fp.write(closing)

main()
