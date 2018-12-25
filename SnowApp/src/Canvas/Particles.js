import { Float32BufferAttribute } from 'three/src/core/BufferAttribute';
import { Color } from 'three/src/math/Color';

export default class Particles {
  constructor(num, w, h, hitCallback) {
    this.num = num;
    this.w   = w;
    this.h   = h;
    const pts = [];
    const cls = [];
    const szs = [];
    const rts = [];
    const tis = [];
    this.velocities  = [];
    this.rotSpeeds   = [];
    this.isHit       = [];
    this.isDead      = [];
    this.hitCallback = hitCallback;
    const c = new Color();

    for(let i = 0; i < this.num; ++i) {
      pts.push((Math.random() * 2 - 1) * this.w);
      pts.push((Math.random() * 2 - 1) * this.h);
      pts.push(Math.random() * -this.h * 0.05);

      this.velocities.push((Math.random() * 2 - 1) * 0.2);
      this.velocities.push(-(Math.random() * 0.5 + 1.0));
      this.velocities.push(0);

      c.setHSL(i / this.num, 1.0, Math.min((Math.random() * 2 - 1) * 0.2 + 0.9, 1.0));
      cls.push(c.r, c.g, c.b, 1.0);

      szs.push(( Math.random() * 2 - 1 ) * 60 + 90);
      rts.push(Math.PI * Math.random());

      this.rotSpeeds.push((Math.random() * 2 - 1) * 0.01);

      tis.push(Math.floor(Math.random() * 7));

      this.isHit.push(false);
      this.isDead.push(false);
    }

    this.positions  = new Float32BufferAttribute(pts, 3).setDynamic(true);
    this.colors     = new Float32BufferAttribute(cls, 4).setDynamic(true);
    this.sizes      = new Float32BufferAttribute(szs, 1).setDynamic(true);
    this.rotations  = new Float32BufferAttribute(rts, 1).setDynamic(true);
    this.texIndices = new Float32BufferAttribute(tis, 1).setDynamic(true);
  }

  update() {
    const hh  = this.h / 2;
    const pts = this.positions.array;
    const szs = this.sizes.array;
    const cls = this.colors.array;
    const rts = this.rotations.array;

    cls[3] -= cls[3] * 0.03;

    for(let i = 0; i < this.num; ++i) {
      // if(this.isDead[i]) continue;

      const x = i * 3;
      const y = i * 3 + 1;
      const a = i * 4 + 3;

      if(this.isHit[i]) {
        const a = i * 4 + 3;
        if(cls[a] > 0.01) {
          cls[a] -= cls[a] * 0.05;
        }
        else {
          cls[a] = 0.0;
          this.isDead[i] = true;
        }
      }
      else {
        if(pts[y] > -hh) {
          pts[x] += this.velocities[x];
          pts[y] += this.velocities[y];
          rts[i] += this.rotSpeeds[i];
        }

        if(pts[y] <= -hh) {
          pts[y]  = -hh;
          cls[a] -= 0.002;

          if(cls[a] < 0.01) {
            this.isDead[i] = true;
          }
        }
      }

      if(this.isDead[i]) {
        this.isHit[i]  = false;
        this.isDead[i] = false;
        pts[x] = (Math.random() * 2 - 1) * this.w;
        pts[y] = Math.random() * hh + hh;
        szs[i] = ( Math.random() * 2 - 1 ) * 60 + 90;
        cls[a] = 1.0;
      }
    }

    this.positions.needsUpdate = true;
    this.sizes.needsUpdate     = true;
    this.colors.needsUpdate    = true;
    this.rotations.needsUpdate = true;
  }

  mouseMoved(_x, _y) {
    const hh = this.h / 2;
    const pts = this.positions.array;
    const cls = this.colors.array;
    const szs = this.sizes.array;
    for(let i = 0; i < this.num; ++i) {
      const x = i * 3;
      const y = i * 3 + 1;
      const r = i * 4;
      const g = i * 4 + 1;
      const b = i * 4 + 2;
      const a = i * 4 + 3;

      const dx = pts[x] - _x;
      const dy = pts[y] - _y;
      const dist        = Math.sqrt(dx * dx + dy * dy);
      const touchRadius = szs[i] * 0.5;
      if(dist < touchRadius) {
        // ヒット時のコールバック
        if(this.hitCallback != undefined) {
          if(!this.isHit[i]) {
            szs[i] *= 6;
            this.isHit[i] = true;
            this.hitCallback(cls[r], cls[g], cls[b]);
          }
        }
      }
    }
    this.sizes.needsUpdate = true;
  }
};
