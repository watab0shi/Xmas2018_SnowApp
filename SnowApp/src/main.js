import QuadWarper from './QuadWarper';
import Canvas from './Canvas';
import size from './config/size';

// ドキュメントを読み込んだらページごとの処理を実行
document.addEventListener('DOMContentLoaded', () => {
  let w = size.w;
  let h = size.h;

  const canvas = new Canvas('box-warp', w, h);

  const warper = new QuadWarper('mapper', w, h);
  // warper.setCorner(44,15,1865,11,0,861,1919,892);// set default corner positions
  // warper.keyPressed(' ');// disable warping

  // websocket
  let ws;
  let reConnectInterval = 1000;
  const initWebSocket = _ => {
    ws = new WebSocket('ws://localhost:9092');

    canvas.hitCallback = (r, g, b) => {
      ws.send(JSON.stringify({ message: "hit", value: { r: r, g: g, b: b } }));
    };

    ws.onopen = e => {
      console.log("[ws:open]");
      ws.send(JSON.stringify({ message: "size", value: { w: w, h: h } }));
    };
    ws.onmessage = e => {
      const data = JSON.parse(e.data);
      // console.log("[ws:message]", data);

      const isMouseEvent = (data.message == "mousePressed" || data.message == "mouseMoved");
      if(isMouseEvent) {
        const x = data.value.x;
        const y = data.value.y;
        canvas.mouseMoved(x, y);
      }

      if(data.message == "touched") {
        // { message: "touched", value: "[x1, y1, x2, y2, x3, y3...]" }
        const arr = JSON.parse(data.value);
        for(let i = 0; i < arr.length; i += 2) {
          const x = arr[i];
          const y = arr[i + 1];
          canvas.mouseMoved(x, y);
        }
      }
    };
    ws.onclose = e => {
      console.log("[ws:close]");
      setTimeout(_ => { initWebSocket(); }, reConnectInterval += 100);
    };
  };

  initWebSocket();

  window.addEventListener('mousedown', e => {
    const x = e.clientX;
    const y = e.clientY;
    warper.mousePressed(x, y);
  });

  window.addEventListener('mouseup', e => {
    warper.mouseReleased();
  })

  window.addEventListener('mousemove', e => {
    const x = e.clientX;
    const y = e.clientY;
    warper.mouseMoved(x, y);
    canvas.mouseMoved(x, y);
  });

  window.addEventListener('keydown', e => {
    warper.keyPressed(e.key);
  });
});