// VIRTUAL DOM
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

// REAL DOM
export function createDomNode(fiber) {
  if (fiber.type == "TEXT_ELEMENT") {
    return document.createTextNode(fiber.props.nodeValue);
  }

  let dom =
    fiber.type == "TEXT_ELEMENT"
      ? document.createTextNode(fiber.props.nodeValue)
      : document.createElement(fiber.type);

  updatedDom(dom, {}, fiber.props);

  return dom;
}

export function updatedDom(dom, prevProps, nextProps) {
  // remove old props
  Object.keys(prevProps || {})
    .filter((key) => key != "children")
    .forEach((name) => {
      dom[name] = "";
    });

  // add new props
  Object.keys(nextProps || {})
    .filter((key) => key != "children")
    .forEach((name) => {
      dom[name] = nextProps[name];
    });
}

// FIBER
let nextUnitOfWork = null;
let wipRoot = null;
let currentRoot = null;
let deletions = null;

let wipFiber = null;
let hookIndex = null;

export function render(element, container) {
  wipRoot = {
    dom: container,
    props: {
      children: [element],
    },
    alternate: currentRoot,
  };

  deletions = [];
  nextUnitOfWork = wipRoot;
}

// SCHEDULER
export function workLoop(deadline) {
  let shouldYield = false;

  while (nextUnitOfWork && !shouldYield) {
    nextUnitOfWork = performUnitOfWork(nextUnitOfWork);
    shouldYield = deadline.timeRemaining() < 1;
  }

  if (!nextUnitOfWork && wipRoot) {
    commitRoot();
  }

  requestIdleCallback(workLoop);
}

requestIdleCallback(workLoop);

// PERFORM UNIT OF WORK
export function performUnitOfWork(fiber) {
  let isFunctionalComponent = typeof fiber.type == "function";

  if (isFunctionalComponent) {
    updateFunctionalComponent(fiber);
  } else {
    updateHostComponent(fiber);
  }

  if (fiber.child) {
    return fiber.child;
  }

  let nextFiber = fiber;

  while (nextFiber) {
    if (nextFiber.sibling) {
      return nextFiber.sibling;
    }

    nextFiber = nextFiber.parent;
  }

  return null;
}

export function updateHostComponent(fiber) {
  // container
  if (!fiber.dom) {
    fiber.dom = createDomNode(fiber);
  }

  let elements = fiber.props.children || [];
  reconcileChildren(fiber, elements);
}

export function updateFunctionalComponent(fiber) {
  wipFiber = fiber;
  hookIndex = 0;
  wipFiber.hooks = [];

  let children = [fiber.type(fiber.props)];
  reconcileChildren(fiber, children);
}

// RECONCILE
function reconcileChildren(wipFiber, elements) {
  let index = 0;
  let prevSibling = null;
  let oldFiber = wipFiber.alternate && wipFiber.alternate.child;

  while (index < elements.length || oldFiber != null) {
    const element = elements[index];
    let newFiber = null;

    const sameType = oldFiber && element && element.type === oldFiber.type;

    // update
    if (sameType) {
      newFiber = {
        type: oldFiber.type,
        props: element.props,
        dom: oldFiber.dom,
        parent: wipFiber,
        alternate: oldFiber,
        effectTag: "UPDATE",
      };
    }

    // placement
    if (element && !sameType) {
      newFiber = {
        type: element.type,
        props: element.props,
        dom: null,
        parent: wipFiber,
        alternate: null,
        effectTag: "PLACEMENT",
      };
    }

    // deletion
    if (oldFiber && !sameType) {
      oldFiber.effectTag = "DELETION";
      deletions.push(oldFiber);
    }

    if (oldFiber) {
      oldFiber = oldFiber.sibling;
    }

    if (index === 0) {
      wipFiber.child = newFiber;
    } else if (element) {
      prevSibling.sibling = newFiber;
    }

    prevSibling = newFiber;
    index++;
  }
}

// COMMIT
export function commitRoot() {
  deletions.forEach(commitWork);
  commitWork(wipRoot.child);
  currentRoot = wipRoot;
  wipRoot = null;
}

function commitWork(fiber) {
  if (!fiber) return;

  let domParentFiber = fiber.parent;
  while (!domParentFiber.dom) {
    domParentFiber = domParentFiber.parent;
  }

  const domParent = domParentFiber.dom;

  if (fiber.effectTag === "PLACEMENT" && fiber.dom != null) {
    domParent.appendChild(fiber.dom);
  } else if (fiber.effectTag === "UPDATE" && fiber.dom != null) {
    updatedDom(fiber.dom, fiber.alternate.props, fiber.props);
  } else if (fiber.effectTag === "DELETION") {
    commitDeletion(fiber, domParent);
  }

  commitWork(fiber.child);
  commitWork(fiber.sibling);
}

function commitDeletion(fiber, domParent) {
  if (fiber.dom) {
    domParent.removeChild(fiber.dom);
  } else {
    commitDeletion(fiber.child, domParent);
  }
}

// useState
export function useState(initial) {
  const oldHook =
    wipFiber.alternate &&
    wipFiber.alternate.hooks &&
    wipFiber.alternate.hooks[hookIndex];

  const hook = {
    state: oldHook ? oldHook.state : initial,
    queue: [],
  };

  const actions = oldHook ? oldHook.queue : [];

  actions.forEach((action) => {
    hook.state = typeof action === "function" ? action(hook.state) : action;
  });

  const setState = (action) => {
    hook.queue.push(action);

    // trigger re-render
    wipRoot = {
      dom: currentRoot.dom,
      props: currentRoot.props,
      alternate: currentRoot,
    };

    nextUnitOfWork = wipRoot;
    deletions = [];
  };

  wipFiber.hooks.push(hook);
  hookIndex++;

  return [hook.state, setState];
}

// HTML formatter
export function html(htmlobj) {
  const tag = Object.keys(htmlobj)[0];
  const { prop, children } = htmlobj[tag];

  const formattedChildren = children.map((child) => {
    if (typeof child === "string" || typeof child === "number") {
      return child;
    }

    return html(child);
  });

  return createElement(tag, prop, ...formattedChildren);
}
