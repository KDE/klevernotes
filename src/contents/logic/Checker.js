
function getLines(text){
    return text.split("\n")
}


function getCursorLineIndex(){
    let textLength = 0
    let index = 0
    for (var i = 0 ; i < lines.length ; i++){
        const line = lines[i]
        const nextTextLength = textLength+line.length
        if (nextTextLength >= cursorPosition) break
        textLength = nextTextLength+1
        index += 1
    }
    return index
}

function getLineIndex(text,cursorPosition){
    const lines = getLines(text)
    return getCursorLineIndex(lines,cursorPosition)
}

function getCursorLine(text,cursorPosition){
    const lines = getLines(text)

    const lineIndex = getCursorLineIndex(lines,cursorPosition)

    return lines[lineIndex]
}

function getLineStart(text) {
    return text.lastIndexOf('\n')+1
}

function getLineEnd(text) {
    return text.indexOf('\n')-1
}
