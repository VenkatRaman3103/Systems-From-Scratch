import { createElement, render } from "./engine.js";

const element = createElement(
  "h1",
  { id: "some" },
  "Hello",
  createElement("h2", null, "world"),
  createElement("p", null, "description"),
);

const containder = document.getElementById("root");

render(element, containder);

// console.log(JSON.stringify(element, null, 2));
