function tokenize(text) {
    let tokens = [];

    let words = text.split(/(\s+|[.,!?])/).filter(t => t.trim() !== "");

    words.forEach(w => {
        if (w.match(/^[a-zA-Z]+$/)) tokens.push({type:"WORD", value:w});
        else if (w.match(/^[0-9]+$/)) tokens.push({type:"NUMBER", value:w});
        else if (w.match(/^@[a-zA-Z0-9]+$/)) tokens.push({type:"MENTION", value:w});
        else if (w.match(/^#[a-zA-Z0-9]+$/)) tokens.push({type:"HASHTAG", value:w});
        else if (w.match(/^[a-zA-Z0-9]+@[a-zA-Z]+\.com$/)) tokens.push({type:"EMAIL", value:w});
        else tokens.push({type:"UNKNOWN", value:w});
    });

    return tokens;
}

function chatbotResponse(tokens) {
    for (let t of tokens) {
        if (t.value.toLowerCase() === "hello") return "Hello! How can I help?";
        if (t.type === "EMAIL") return "I detected an email address.";
        if (t.type === "HASHTAG") return "Looks like a hashtag!";
    }
    return "I processed your input using automata-based tokenization.";
}

function send() {
    let input = document.getElementById("input").value;

    let tokens = tokenize(input);
    let response = chatbotResponse(tokens);

    let chat = document.getElementById("chat");

    chat.innerHTML += "<p><b>You:</b> " + input + "</p>";
    chat.innerHTML += "<p><b>Tokens:</b> " + JSON.stringify(tokens) + "</p>";
    chat.innerHTML += "<p><b>Bot:</b> " + response + "</p>";

    document.getElementById("input").value = "";
}

// Helper functions for rendering token bar charts and entity lists
function renderTokenBarChartHelper(tokens) {
    const tokenColors = {
        'WORD': { bg: '#f0f4ff', fg: '#667eea' },
        'NUMBER': { bg: '#fff4e6', fg: '#ff9800' },
        'EMAIL': { bg: '#f3e5f5', fg: '#9c27b0' },
        'URL': { bg: '#e0f2f1', fg: '#009688' },
        'HASHTAG': { bg: '#fce4ec', fg: '#e91e63' },
        'MENTION': { bg: '#e3f2fd', fg: '#2196f3' },
        'UNKNOWN': { bg: '#eeeeee', fg: '#757575' }
    };

    const typeCounts = {};
    tokens.forEach(token => {
        if (token.type !== 'WHITESPACE') {
            typeCounts[token.type] = (typeCounts[token.type] || 0) + 1;
        }
    });

    const types = Object.keys(typeCounts);
    if (types.length === 0) return null;

    const maxCount = Math.max(...Object.values(typeCounts));
    return types.map(type => ({
        type: type,
        count: typeCounts[type],
        percentage: (typeCounts[type] / maxCount) * 100,
        colors: tokenColors[type] || tokenColors['UNKNOWN']
    }));
}

function renderEntityListHelper(tokens) {
    return tokens.filter(token =>
        ['EMAIL', 'URL', 'HASHTAG', 'MENTION'].includes(token.type)
    );
}
