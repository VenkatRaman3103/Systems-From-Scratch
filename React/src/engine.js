export function createTextElement(text) {
  return {
    type: "TEXT_ELEMENT",
    props: {
      nodeValue: text,
      children: [],
    },
  };
}

export function createElement(type, props, ...children) {
  return {
    type: type,
    props: {
      ...props,
      children: children
        .flat()
        .map((child) =>
          typeof child == "object" ? child : createTextElement(child),
        ),
    },
  };
}

export function createDomNode(vnode) {
  if (vnode.type == "TEXT_ELEMENT") {
    return document.createTextNode(vnode.props.nodeValue);
  }

  let dom = document.createElement(vnode.type);

  Object.keys(vnode.props || {})
    .filter((key) => key != "children")
    .forEach((name) => {
      console.log(name);
      dom[name] = vnode.props[name];
    });

  return dom;
}

export function render(vnode, container) {
  let dom = createDomNode(vnode);

  vnode.props.children.forEach((child) => {
    render(child, dom);
  });

  container.appendChild(dom);
}
