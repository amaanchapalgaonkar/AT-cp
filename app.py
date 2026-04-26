from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess
import re

app = Flask(__name__)
CORS(app)

def parse_tokens(output):
    tokens = []

    lines = output.strip().split("\n")

    for line in lines:
        if ":" in line:
            parts = line.split(":", 1)
            token_type = parts[0].strip()
            value = parts[1].strip()

            # Include all token types: WORD, EMAIL, URL, HASHTAG, MENTION, NUMBER, PUNCT, WHITESPACE, UNKNOWN
            if token_type in ["WORD", "EMAIL", "URL", "HASHTAG", "MENTION", "NUMBER", "PUNCT", "WHITESPACE", "UNKNOWN"]:
                tokens.append({
                    "type": token_type,
                    "value": value
                })

    return tokens

def chatbot_response(tokens):
    responses = []

    for token in tokens:
        t = token["type"]
        val = token["value"].lower()

        if t == "WORD" and val == "hello":
            responses.append("Hello! How can I help?")
        if t == "EMAIL":
            responses.append("I detected an email address.")
        if t == "URL":
            responses.append("This looks like a link.")
        if t == "HASHTAG":
            responses.append("This is a hashtag topic.")
        if t == "MENTION":
            responses.append("User mention detected.")

    return " ".join(responses) if responses else "Input processed successfully."

@app.route("/tokenize", methods=["POST"])
def tokenize():
    data = request.json
    text = data.get("text", "")

    if not text.strip():
        return jsonify({
            "tokens": [],
            "response": "Please enter some text to tokenize."
        })

    try:
        result = subprocess.run(
            ["tokenizer.exe", text],
            capture_output=True,
            text=True,
            cwd="."
        )

        output = result.stdout
        tokens = parse_tokens(result.stdout)
        response = chatbot_response(tokens)

        return jsonify({
            "tokens": tokens,
            "response": response
        })

    except Exception as e:
        return jsonify({
            "error": str(e),
            "tokens": [],
            "response": "Error running tokenizer."
        })

if __name__ == "__main__":
    app.run(debug=True)