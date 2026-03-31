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

