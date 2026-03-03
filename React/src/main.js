import { createElement, render, useState } from "./engine.js";

function Counter() {
  const [count, setCount] = useState(0);
  const [text, setText] = useState(true);

  return createElement(
    "div",
    null,
    createElement("h1", null, count),
    createElement("p", null, text ? "hello" : "world"),
    createElement(
      "button",
      { onclick: () => setCount((c) => c + 1) },
      "Increment",
    ),
    createElement("button", { onclick: () => setText(!text) }, "Change Text"),
  );
}

const container = document.getElementById("root");
render(createElement(Counter, null), container);
