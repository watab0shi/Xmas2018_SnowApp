import { transform2d } from './homograohyUtils';

export default class QuadWarper {
  constructor(mapperId, w, h) {
    this.w = w;
    this.h = h;

    this.currectCorner = -1;
    this.corners = [
      [0, 0],
      [w, 0],
      [0, h],
      [w, h]
    ];

    this.mapperElement  = document.getElementById(mapperId);
    this.boxElement     = this.mapperElement.querySelector("#box-warp");
    this.cornerElements = this.mapperElement.querySelectorAll('.handle');
    this.disabled       = false;

    this.update();
  }

  dumpCorners() {
    console.log(this.corners.toString());
  }

  setCorner(x1, y1, x2, y2, x3, y3, x4, y4) {
    this.corners[0][0] = x1;
    this.corners[0][1] = y1;
    this.corners[1][0] = x2;
    this.corners[1][1] = y2;
    this.corners[2][0] = x3;
    this.corners[2][1] = y3;
    this.corners[3][0] = x4;
    this.corners[3][1] = y4;
    this.update();
  }

  update() {
    const c = this.corners;
    transform2d(this.boxElement, c[0][0], c[0][1], c[1][0], c[1][1],
                                 c[2][0], c[2][1], c[3][0], c[3][1]);
    for (let i = 0; i < 4; ++i) {
      this.cornerElements[i].style.left = c[i][0] + "px";
      this.cornerElements[i].style.top  = c[i][1] + "px";
    }
    // this.dumpCorners();
  }

  move(x, y) {
    if (this.currectCorner < 0) return;
    const curCorner = this.corners[this.currectCorner];
    curCorner[0]    = x;
    curCorner[1]    = y;
    this.update();
  }

  mousePressed(x, y) {
    if(this.disabled) return;
    
    let dx, dy, dist;
    let handleRadius = 50;
    
    this.currectCorner = -1;
    for(let i = 0; i < this.cornerElements.length; ++i) {
      this.cornerElements[i].classList.remove('is-selected');
    }

    for(let i = 0; i < 4; ++i) {
      dx = x - this.corners[i][0];
      dy = y - this.corners[i][1];
      dist = Math.sqrt(dx * dx + dy * dy);
      if (dist <= handleRadius) {
        this.currectCorner = i;
        this.cornerElements[i].classList.add('is-selected');
        break;
      }
    }
  }

  mouseReleased() {
    this.currectCorner = -1;
  }

  mouseMoved(x, y) {
    if(this.disabled) return;

    this.move(x, y);
  }

  keyPressed(key) {
    // toggle enable
    if(key == " ") {
      this.disabled = !this.disabled;
      if(this.disabled) this.mapperElement.classList.add('is-disabled');
      else              this.mapperElement.classList.remove('is-disabled');
    }

    if(key == "Enter") {
      this.dumpCorners();
    }
  }
};
