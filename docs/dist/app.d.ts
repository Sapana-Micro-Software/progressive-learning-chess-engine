declare class BackgroundCanvas {
    private canvas;
    private ctx;
    private particles;
    private animationId;
    constructor(canvasId: string);
    private resize;
    private initParticles;
    private animate;
    private drawConnections;
}
declare class Particle {
    x: number;
    y: number;
    vx: number;
    vy: number;
    radius: number;
    private maxX;
    private maxY;
    constructor(x: number, y: number, maxX: number, maxY: number);
    update(): void;
    draw(ctx: CanvasRenderingContext2D): void;
}
declare class NetworkVisualization {
    private canvas;
    private ctx;
    private nodes;
    private connections;
    private animationId;
    constructor(canvasId: string);
    private initNetwork;
    private animate;
}
declare class NetworkNode {
    x: number;
    y: number;
    color: string;
    label: string;
    pulse: number;
    constructor(x: number, y: number, color: string, label: string);
    draw(ctx: CanvasRenderingContext2D): void;
}
declare class Connection {
    from: number;
    to: number;
    pulse: number;
    constructor(from: number, to: number);
    update(): void;
    draw(ctx: CanvasRenderingContext2D, nodes: NetworkNode[]): void;
}
declare class ArchitectureDiagram {
    private canvas;
    private ctx;
    constructor(canvasId: string);
    private draw;
    private drawComponent;
}
declare function initSmoothScroll(): void;
declare function initScrollAnimations(): void;
declare function initMobileMenu(): void;
//# sourceMappingURL=app.d.ts.map