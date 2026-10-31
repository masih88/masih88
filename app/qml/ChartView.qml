import QtQuick

// Draws a chart from the core's chart model (axes, lines, labels, markers) on a Canvas.
// Tapping inside the plot area emits picked(x, y) in data units (e.g. set state 1 in the h,x chart).
Item {
    id: view
    property var chart: ({})
    property bool pickable: false
    signal picked(real x, real y)

    readonly property bool dark: Calc.darkTheme
    readonly property bool valid: chart.xmax !== undefined && chart.xmax > chart.xmin && chart.ymax > chart.ymin
    readonly property real padL: 34
    readonly property real padR: 10
    readonly property real padT: 10
    readonly property real padB: 38

    function px(x) { return padL + (x - chart.xmin) / (chart.xmax - chart.xmin) * (width - padL - padR) }
    function py(y) { return padT + (chart.ymax - y) / (chart.ymax - chart.ymin) * (height - padT - padB) }
    function dataX(p) { return chart.xmin + (p - padL) / (width - padL - padR) * (chart.xmax - chart.xmin) }
    function dataY(p) { return chart.ymax - (p - padT) / (height - padT - padB) * (chart.ymax - chart.ymin) }

    // pen per series style: [colour light, colour dark, width, dashed]
    function pen(style) {
        switch (style) {
        case "t":       return ["#CBD3DE", "#2E3848", 1, false]
        case "h":       return ["#DDBF95", "#5C4B33", 1, true]
        case "rh":      return ["#8FB3E0", "#3C6496", 1, false]
        case "sat":     return ["#1F4E89", "#9CC3F5", 2.5, false]
        case "process": return ["#D9741C", "#F2A900", 3, false]
        case "aux":     return ["#D9741C", "#F2A900", 1.5, true]
        default:        return ["#888888", "#888888", 1, false]
        }
    }
    function labelColor(style) {
        switch (style) {
        case "rh":  return dark ? "#7FA6D8" : "#4A78B5"
        case "sat": return dark ? "#9CC3F5" : "#1F4E89"
        case "h":
        case "hTitle": return dark ? "#B39466" : "#9A7442"
        default:    return dark ? "#C9D3E0" : "#4A5B72"
        }
    }

    function paint(ctx) {
        const w = width, h = height
        ctx.reset()
        ctx.clearRect(0, 0, w, h)
        if (!valid)
            return
        const left = padL, top = padT, right = w - padR, bottom = h - padB
        const textColor = dark ? "#C9D3E0" : "#4A5B72"

        // plot area
        ctx.fillStyle = dark ? "#151B26" : "#FFFFFF"
        ctx.fillRect(left, top, right - left, bottom - top)

        // series (clipped to the plot area)
        ctx.save()
        ctx.beginPath()
        ctx.rect(left, top, right - left, bottom - top)
        ctx.clip()
        const order = ["t", "h", "rh", "sat", "aux", "process"]
        for (let o = 0; o < order.length; ++o) {
            for (let i = 0; i < chart.series.length; ++i) {
                const s = chart.series[i]
                if (s.style !== order[o])
                    continue
                const p = pen(s.style)
                ctx.strokeStyle = dark ? p[1] : p[0]
                ctx.lineWidth = p[2]
                ctx.lineJoin = "round"
                if (ctx.setLineDash)
                    ctx.setLineDash(p[3] ? [5, 4] : [])
                ctx.beginPath()
                for (let k = 0; k + 1 < s.pts.length; k += 2) {
                    if (k === 0)
                        ctx.moveTo(px(s.pts[k]), py(s.pts[k + 1]))
                    else
                        ctx.lineTo(px(s.pts[k]), py(s.pts[k + 1]))
                }
                ctx.stroke()
            }
        }
        if (ctx.setLineDash)
            ctx.setLineDash([])

        // curve labels
        for (let i = 0; i < chart.labels.length; ++i) {
            const l = chart.labels[i]
            ctx.fillStyle = labelColor(l.style)
            ctx.font = (l.style === "sat" ? "bold " : "") + "10px sans-serif"
            if (l.style === "h" || l.style === "hTitle") {
                ctx.textAlign = "left"
                ctx.textBaseline = "top"
                ctx.fillText(l.text, px(l.x) + 3, py(l.y) + 2)
            } else {
                ctx.textAlign = "right"
                ctx.textBaseline = "bottom"
                ctx.fillText(l.text, px(l.x) - 3, py(l.y) - 1)
            }
        }
        ctx.restore()

        // frame, ticks and axis labels
        ctx.strokeStyle = dark ? "#4A5568" : "#9AA6B6"
        ctx.lineWidth = 1
        ctx.strokeRect(left, top, right - left, bottom - top)
        ctx.fillStyle = textColor
        ctx.font = "10px sans-serif"
        ctx.textAlign = "center"
        ctx.textBaseline = "top"
        for (let x = Math.ceil(chart.xmin / chart.xstep) * chart.xstep; x <= chart.xmax + 1e-9; x += chart.xstep) {
            ctx.beginPath()
            ctx.moveTo(px(x), bottom)
            ctx.lineTo(px(x), bottom + 4)
            ctx.stroke()
            ctx.fillText(Math.round(x * 10) / 10, px(x), bottom + 5)
        }
        ctx.textAlign = "right"
        ctx.textBaseline = "middle"
        for (let y = Math.ceil(chart.ymin / chart.ystep) * chart.ystep; y <= chart.ymax + 1e-9; y += chart.ystep) {
            ctx.beginPath()
            ctx.moveTo(left - 4, py(y))
            ctx.lineTo(left, py(y))
            ctx.stroke()
            ctx.fillText(Math.round(y * 10) / 10, left - 6, py(y))
        }
        ctx.font = "11px sans-serif"
        ctx.textAlign = "center"
        ctx.textBaseline = "bottom"
        ctx.fillText(chart.xtitle, (left + right) / 2, h - 2)
        ctx.save()
        ctx.translate(10, (top + bottom) / 2)
        ctx.rotate(-Math.PI / 2)
        ctx.textBaseline = "middle"
        ctx.fillText(chart.ytitle, 0, 0)
        ctx.restore()

        // markers on top
        for (let i = 0; i < chart.markers.length; ++i) {
            const m = chart.markers[i]
            const x = px(m.x), y = py(m.y)
            const accent = dark ? "#F2A900" : "#D9741C"
            ctx.beginPath()
            if (m.style === "state") {
                ctx.arc(x, y, 6, 0, 2 * Math.PI)
                ctx.fillStyle = dark ? "#F2C14E" : "#1F3A5F"
                ctx.fill()
                ctx.lineWidth = 2
                ctx.strokeStyle = dark ? "#151B26" : "#FFFFFF"
                ctx.stroke()
            } else {
                ctx.arc(x, y, 4, 0, 2 * Math.PI)
                ctx.fillStyle = dark ? "#151B26" : "#FFFFFF"
                ctx.fill()
                ctx.lineWidth = 2
                ctx.strokeStyle = accent
                ctx.stroke()
            }
            ctx.font = "bold 12px sans-serif"
            ctx.textAlign = "left"
            ctx.textBaseline = "bottom"
            ctx.fillStyle = m.style === "state" ? (dark ? "#F2C14E" : "#1F3A5F") : accent
            ctx.fillText(m.label, x + 7, y - 4)
        }
    }

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: view.paint(getContext("2d"))
    }

    onChartChanged: canvas.requestPaint()
    onDarkChanged: canvas.requestPaint()
    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()

    TapHandler {
        enabled: view.pickable && view.valid
        onTapped: function (eventPoint) {
            const p = eventPoint.position
            if (p.x >= view.padL && p.x <= view.width - view.padR && p.y >= view.padT && p.y <= view.height - view.padB)
                view.picked(view.dataX(p.x), view.dataY(p.y))
        }
    }
}
