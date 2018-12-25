import { WebGLRenderer } from 'three/src/renderers/WebGLRenderer';
import { PerspectiveCamera } from 'three/src/cameras/PerspectiveCamera';
import { Scene } from 'three/src/scenes/Scene';
import { Vector2 } from 'three/src/math/Vector2';
import { AdditiveBlending } from 'three/src/constants.js';
import { BufferGeometry } from 'three/src/core/BufferGeometry';
import { ShaderMaterial } from 'three/src/materials/ShaderMaterial';
import { Points } from 'three/src/objects/Points';
import { TextureLoader } from 'three/src/loaders/TextureLoader';

import vertSrc from './shader/vert.glsl';
import fragSrc from './shader/frag.glsl';
import Particles from './Particles';

const setAttributes = (geo, ptcls) => {
  geo.addAttribute('position'    , ptcls.positions);
  geo.addAttribute('color_'      , ptcls.colors);
  geo.addAttribute('size'        , ptcls.sizes);
  geo.addAttribute('rotation'    , ptcls.rotations);
  geo.addAttribute('textureIndex', ptcls.texIndices);
};//

export default class Canvas {
  constructor(containerId, w, h) {
    // マウス座標
    this.mouse = new Vector2(0, 0);

    this.hitCallback = undefined;

    // ウィンドウサイズ
    this.w = w;
    this.h = h;

    // レンダラーを作成
    this.renderer = new WebGLRenderer();
    this.renderer.setSize(this.w, this.h);// 描画サイズ
    this.renderer.setPixelRatio(window.devicePixelRatio);// ピクセル比

    // #containerId にレンダラーの canvas を追加
    const container = document.getElementById(containerId);
    container.appendChild(this.renderer.domElement);

    const fov    = 60;
    const fovRad = (fov / 2) * (Math.PI / 180);// 視野角をラジアンに変換
    const dist   = (this.h / 2) / Math.tan(fovRad);// ウィンドウぴったりのカメラ距離

    // カメラを作成 (視野角, 画面のアスペクト比, カメラに映る最短距離, カメラに映る最遠距離)
    this.camera = new PerspectiveCamera(fov, this.w / this.h, 1, dist * 2);
    this.camera.position.z = dist;// カメラを遠ざける

    // パーティクル
    this.particles = new Particles(700, this.w, this.h, (r, g, b) => {
      if(this.hitCallback != undefined) this.hitCallback(r, g, b);
    });

    // バッファジオメトリ
    const geo = new BufferGeometry();
    setAttributes(geo, this.particles);

    // シェーダーマテリアル
    const mat = new ShaderMaterial({
      uniforms : {
        tex00: { value: new TextureLoader().load('resource/images/snow-00.png') },
        tex01: { value: new TextureLoader().load('resource/images/snow-01.png') },
        tex02: { value: new TextureLoader().load('resource/images/snow-02.png') },
        tex03: { value: new TextureLoader().load('resource/images/snow-03.png') },
        tex04: { value: new TextureLoader().load('resource/images/snow-04.png') },
        tex05: { value: new TextureLoader().load('resource/images/snow-05.png') },
        tex06: { value: new TextureLoader().load('resource/images/snow-06.png') }
      },
      vertexShader  : vertSrc,
      fragmentShader: fragSrc,
      blending      : AdditiveBlending,
      depthTest     : false,
      transparent   : false,
      vertexColors  : true
    });

    // GL_POINT
    this.points  = new Points(geo, mat);

    // シーンを作成
    this.scene = new Scene();
    this.scene.add(this.points);

     // 描画ループ開始
    this.render();
  }

  render() {
    // 次のフレームを要求
    requestAnimationFrame(() => { this.render(); });

    // パーティクルを更新
    this.particles.update();

    // 画面に表示
    this.renderer.render(this.scene, this.camera);
  }

  mouseMoved(x, y) {
    this.mouse.x =  x - (this.w / 2);// 原点を中心に持ってくる
    this.mouse.y = -y + (this.h / 2);// 軸を反転して原点を中心に持ってくる

    this.particles.mouseMoved(this.mouse.x, this.mouse.y);
  }
};
