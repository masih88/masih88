import QtQuick

// Draws a chart from the core's chart model (axes, lines, labels, markers) on a Canvas.
// chart.diagram = true draws a schematic instead (no axes, same scale in x and y, see chart.hpp).
// Tapping inside the plot area emits picked(x, y) in data units (e.g. set state 1 in the h,x chart).
Item {
    id: view
    property var chart: ({})
    property bool pickable: false
    signal picked(real x, real y)

    readonly property bool dark: Calc.darkTheme
    readonly property bool valid: chart.xmax !== undefined && chart.xmax > chart.xmin && chart.ymax > chart.ymin
    readonly property bool diagram: chart.diagram === true
    readonly property real padL: diagram ? 6 : 34
    readonly property real padR: diagram ? 6 : 10
    readonly property real padT: diagram ? 6 : 10
    readonly property real padB: diagram ? 6 : 38
    // schematic: same scale in x and y, drawing centred in the view
    readonly property real dScale: valid ? Math.min((width - padL - padR) / (chart.xmax - chart.xmin),
                                                     (height - padT - padB) / (chart.ymax - chart.ymin)) : 1
    readonly property real dOffX: valid ? (width - dScale * (chart.xmax - chart.xmin)) / 2 : 0
    readonly property real dOffY: valid ? (height - dScale * (chart.ymax - chart.ymin)) / 2 : 0

    readonly property bool xlog: chart.xlog === true && chart.xmin > 0
    readonly property bool ylog: chart.ylog === true && chart.ymin > 0
    function lg(v) { return Math.log(v) / Math.LN10 }
    // fraction 0…1 of a value along an axis (linear or logarithmic)
    function fx(x) { return xlog ? (lg(Math.max(x, 1e-300)) - lg(chart.xmin)) / (lg(chart.xmax) - lg(chart.xmin)) : (x - chart.xmin) / (chart.xmax - chart.xmin) }
    function fy(y) { return ylog ? (lg(Math.max(y, 1e-300)) - lg(chart.ymin)) / (lg(chart.ymax) - lg(chart.ymin)) : (y - chart.ymin) / (chart.ymax - chart.ymin) }

    function px(x) {
        if (diagram)
            return dOffX + (x - chart.xmin) * dScale
        return padL + fx(x) * (width - padL - padR)
    }
    function py(y) {
        if (diagram)
            return dOffY + (chart.ymax - y) * dScale
        return padT + (1 - fy(y)) * (height - padT - padB)
    }
    function dataX(p) {
        const f = (p - padL) / (width - padL - padR)
        return xlog ? Math.pow(10, lg(chart.xmin) + f * (lg(chart.xmax) - lg(chart.xmin))) : chart.xmin + f * (chart.xmax - chart.xmin)
    }
    function dataY(p) {
        const f = 1 - (p - padT) / (height - padT - padB)
        return ylog ? Math.pow(10, lg(chart.ymin) + f * (lg(chart.ymax) - lg(chart.ymin))) : chart.ymin + f * (chart.ymax - chart.ymin)
    }
    // tick values: every step (linear) or every decade (log)
    function ticks(min, max, step, log) {
        const t = []
        if (log) {
            for (let e = Math.ceil(lg(min) - 1e-9); e <= lg(max) + 1e-9; ++e)
                t.push(Math.pow(10, e))
        } else {
            for (let v = Math.ceil(min / step) * step; v <= max + 1e-9; v += step)
                t.push(v)
        }
        return t
    }
    function tickText(v, log) {
        if (!log)
            return Math.round(v * 10) / 10
        if (v >= 1e4)
            return (v / 1000) + "k"
        return v >= 1 ? Math.round(v) : Number(v.toPrecision(1))
    }

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

    // Schematic (hydraulic circuits): pipes, symbols, centred labels and round markers, no axes.
    function paintDiagram(ctx) {
        const k = dScale
        const ink = dark ? "#D5DDE8" : "#26323F"
        const bg = dark ? "#151B26" : "#FFFFFF"
        ctx.fillStyle = bg
        ctx.fillRect(0, 0, width, height)
        ctx.lineJoin = "round"
        ctx.lineCap = "round"

        for (let i = 0; i < chart.series.length; ++i) {
            const s = chart.series[i]
            if (s.pts.length < 4)
                continue
            ctx.beginPath()
            for (let j = 0; j + 1 < s.pts.length; j += 2) {
                if (j === 0)
                    ctx.moveTo(px(s.pts[j]), py(s.pts[j + 1]))
                else
                    ctx.lineTo(px(s.pts[j]), py(s.pts[j + 1]))
            }
            if (ctx.setLineDash)
                ctx.setLineDash(s.style === "sig" ? [Math.max(3, 1.4 * k), Math.max(2, k)] : [])
            if (s.style.indexOf("fill:") === 0) {  // filled with the given colour (colour bands), outlined
                ctx.fillStyle = s.style.substring(5)
                ctx.fill()
                ctx.strokeStyle = ink
                ctx.lineWidth = Math.max(1, 0.3 * k)
                ctx.stroke()
                continue
            }
            if (s.style.indexOf("line:") === 0) {  // thick line in the given colour (bridges, wires)
                ctx.strokeStyle = s.style.substring(5)
                ctx.lineWidth = Math.max(2, 0.7 * k)
                ctx.stroke()
                continue
            }
            switch (s.style) {
            case "vl":
            case "rl":
            case "pipe":
                ctx.strokeStyle = s.style === "vl" ? (dark ? "#EF5350" : "#D32F2F")
                                : s.style === "rl" ? (dark ? "#64A0F0" : "#1E5AA8")
                                : (dark ? "#8A96A8" : "#6B7785")
                ctx.lineWidth = Math.max(2, 0.85 * k)
                ctx.stroke()
                break
            case "inkfill":
                ctx.fillStyle = ink
                ctx.fill()
                ctx.strokeStyle = ink
                ctx.lineWidth = Math.max(1, 0.3 * k)
                ctx.stroke()
                break
            case "bgfill":
                ctx.fillStyle = bg
                ctx.fill()
                ctx.strokeStyle = ink
                ctx.lineWidth = Math.max(1.2, 0.4 * k)
                ctx.stroke()
                break
            case "sig":
                ctx.strokeStyle = dark ? "#F2A900" : "#D9741C"
                ctx.lineWidth = Math.max(1, 0.35 * k)
                ctx.stroke()
                break
            default: // "ink"
                ctx.strokeStyle = ink
                ctx.lineWidth = Math.max(1.2, 0.4 * k)
                ctx.stroke()
            }
        }
        if (ctx.setLineDash)
            ctx.setLineDash([])

        // markers: actuator (M) and temperature sensor (T)
        for (let i = 0; i < chart.markers.length; ++i) {
            const m = chart.markers[i]
            const x = px(m.x), y = py(m.y)
            const r = Math.max(7, 3.4 * k)
            ctx.beginPath()
            ctx.arc(x, y, r, 0, 2 * Math.PI)
            if (m.style === "motor") {
                ctx.fillStyle = dark ? "#F2A900" : "#D9741C"
                ctx.fill()
                ctx.fillStyle = "#FFFFFF"
            } else {
                ctx.fillStyle = bg
                ctx.fill()
                ctx.strokeStyle = ink
                ctx.lineWidth = Math.max(1.2, 0.4 * k)
                ctx.stroke()
                ctx.fillStyle = ink
            }
            ctx.font = "bold " + Math.round(r * 1.15) + "px sans-serif"
            ctx.textAlign = "center"
            ctx.textBaseline = "middle"
            ctx.fillText(m.label, x, y + 0.5)
        }

        // labels: "c" centred, "l" left, "r" right aligned; second letter "b" = bold
        const fs = Math.round(Math.max(9, Math.min(15, 3.1 * k)))
        for (let i = 0; i < chart.labels.length; ++i) {
            const l = chart.labels[i]
            ctx.fillStyle = ink
            ctx.font = (l.style.length > 1 && l.style.charAt(1) === "b" ? "bold " : "") + fs + "px sans-serif"
            ctx.textAlign = l.style.charAt(0) === "l" ? "left" : l.style.charAt(0) === "r" ? "right" : "center"
            ctx.textBaseline = "middle"
            ctx.fillText(l.text, px(l.x), py(l.y))
        }
    }

    function paint(ctx) {
        const w = width, h = height
        ctx.reset()
        ctx.clearRect(0, 0, w, h)
        if (!valid)
            return
        if (diagram) {
            paintDiagram(ctx)
            return
        }
        const left = padL, top = padT, right = w - padR, bottom = h - padB
        const textColor = dark ? "#C9D3E0" : "#4A5B72"

        // plot area
        ctx.fillStyle = dark ? "#151B26" : "#FFFFFF"
        ctx.fillRect(left, top, right - left, bottom - top)
        const xt = ticks(chart.xmin, chart.xmax, chart.xstep, xlog)
        const yt = ticks(chart.ymin, chart.ymax, chart.ystep, ylog)
        // light decade grid for log charts
        if (xlog || ylog) {
            ctx.save()
            ctx.strokeStyle = dark ? "#2A3445" : "#E6EAF0"
            ctx.lineWidth = 1
            for (let i = 0; xlog && i < xt.length; ++i) {
                ctx.beginPath(); ctx.moveTo(px(xt[i]), top); ctx.lineTo(px(xt[i]), bottom); ctx.stroke()
            }
            for (let i = 0; ylog && i < yt.length; ++i) {
                ctx.beginPath(); ctx.moveTo(left, py(yt[i])); ctx.lineTo(right, py(yt[i])); ctx.stroke()
            }
            ctx.restore()
        }

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
        for (let i = 0; i < xt.length; ++i) {
            const x = xt[i]
            ctx.beginPath()
            ctx.moveTo(px(x), bottom)
            ctx.lineTo(px(x), bottom + 4)
            ctx.stroke()
            ctx.fillText(tickText(x, xlog), px(x), bottom + 5)
        }
        ctx.textAlign = "right"
        ctx.textBaseline = "middle"
        for (let i = 0; i < yt.length; ++i) {
            const y = yt[i]
            ctx.beginPath()
            ctx.moveTo(left - 4, py(y))
            ctx.lineTo(left, py(y))
            ctx.stroke()
            ctx.fillText(tickText(y, ylog), left - 6, py(y))
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
        enabled: view.pickable && view.valid && !view.diagram
        onTapped: function (eventPoint) {
            const p = eventPoint.position
            if (p.x >= view.padL && p.x <= view.width - view.padR && p.y >= view.padT && p.y <= view.height - view.padB)
                view.picked(view.dataX(p.x), view.dataY(p.y))
        }
    }
}
