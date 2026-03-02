import { createElement } from "./engine.js";

const element = createElement(
  "h1",
  null,
  "Hello",
  createElement("h2", null, "world"),
  createElement("p", null, "description"),
);

console.log(JSON.stringify(element, null, 2));
