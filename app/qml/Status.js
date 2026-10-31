.pragma library

// Message severity → [background, foreground, symbol] for light and dark theme
function colors(sev, dark) {
    switch (sev) {
    case "ok":      return dark ? ["#1D3B2A", "#8FE0AE", "✓"] : ["#E3F4EA", "#1E6B3C", "✓"]
    case "warning": return dark ? ["#3D3215", "#F5D27A", "!"] : ["#FFF4D6", "#7A5A00", "!"]
    case "error":   return dark ? ["#45201C", "#F4A69C", "✕"] : ["#FDE4E1", "#9B2C1F", "✕"]
    default:        return dark ? ["#1B2D45", "#9CC3F5", "i"] : ["#E4EEFB", "#1F4E89", "i"]
    }
}

// Worst severity of a message list: "none", "info", "ok", "warning" or "error"
function worst(messages) {
    const order = ["info", "ok", "warning", "error"]
    let w = -1
    for (let i = 0; i < (messages || []).length; ++i)
        w = Math.max(w, order.indexOf(messages[i].severity))
    return w < 0 ? "none" : order[w]
}
