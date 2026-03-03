import { createElement, render, useState } from "./engine.js";

function Counter() {
  const [count, setCount] = useState(0);

  return createElement(
    "div",
    null,
    createElement("h1", null, count),
    createElement(
      "button",
      {
        onclick: () => setCount((c) => c + 1),
      },
      "Increment",
    ),
  );
}

const container = document.getElementById("root");
render(createElement(Counter, null), container);
