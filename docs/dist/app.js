// Copyright (C) 2025, Shyamal Suhana Chandra
// Background Canvas Animation
class BackgroundCanvas {
    constructor(canvasId) {
        this.canvas = document.getElementById(canvasId);
        if (!this.canvas) return;
        this.ctx = this.canvas.getContext('2d');
        this.particles = [];
        this.resize();
        this.initParticles();
        this.animate();
        window.addEventListener('resize', () => this.resize());
    }
    resize() {
        this.canvas.width = window.innerWidth;
        this.canvas.height = window.innerHeight;
    }
    initParticles() {
        const count = Math.floor((this.canvas.width * this.canvas.height) / 15000);
        this.particles = [];
        for (let i = 0; i < count; i++) {
            this.particles.push(new Particle(
                Math.random() * this.canvas.width,
                Math.random() * this.canvas.height,
                this.canvas.width,
                this.canvas.height
            ));
        }
    }
    animate() {
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        this.particles.forEach(p => { p.update(); p.draw(this.ctx); });
        this.drawConnections();
        requestAnimationFrame(() => this.animate());
    }
    drawConnections() {
        for (let i = 0; i < this.particles.length; i++) {
            for (let j = i + 1; j < this.particles.length; j++) {
                const dx = this.particles[i].x - this.particles[j].x;
                const dy = this.particles[i].y - this.particles[j].y;
                const distance = Math.sqrt(dx * dx + dy * dy);
                if (distance < 150) {
                    this.ctx.strokeStyle = `rgba(99, 102, 241, ${0.2 * (1 - distance / 150)})`;
                    this.ctx.lineWidth = 1;
                    this.ctx.beginPath();
                    this.ctx.moveTo(this.particles[i].x, this.particles[i].y);
                    this.ctx.lineTo(this.particles[j].x, this.particles[j].y);
                    this.ctx.stroke();
                }
            }
        }
    }
}
class Particle {
    constructor(x, y, maxX, maxY) {
        this.x = x; this.y = y;
        this.vx = (Math.random() - 0.5) * 0.5;
        this.vy = (Math.random() - 0.5) * 0.5;
        this.radius = Math.random() * 2 + 1;
        this.maxX = maxX; this.maxY = maxY;
    }
    update() {
        this.x += this.vx; this.y += this.vy;
        if (this.x < 0 || this.x > this.maxX) this.vx *= -1;
        if (this.y < 0 || this.y > this.maxY) this.vy *= -1;
    }
    draw(ctx) {
        ctx.fillStyle = 'rgba(99, 102, 241, 0.5)';
        ctx.beginPath();
        ctx.arc(this.x, this.y, this.radius, 0, Math.PI * 2);
        ctx.fill();
    }
}
class NetworkVisualization {
    constructor(canvasId) {
        this.canvas = document.getElementById(canvasId);
        if (!this.canvas) return;
        this.ctx = this.canvas.getContext('2d');
        this.nodes = []; this.connections = [];
        this.initNetwork();
        this.animate();
    }
    initNetwork() {
        const cx = this.canvas.width / 2, cy = this.canvas.height / 2;
        for (let i = 0; i < 8; i++) {
            this.nodes.push(new NetworkNode(cx - 200, cy - 120 + i * 30, '#6366f1', 'Input'));
        }
        this.nodes.push(new NetworkNode(cx - 50, cy - 60, '#8b5cf6', 'Bayesian'));
        this.nodes.push(new NetworkNode(cx - 50, cy, '#8b5cf6', 'Bayesian'));
        this.nodes.push(new NetworkNode(cx - 50, cy + 60, '#8b5cf6', 'Bayesian'));
        this.nodes.push(new NetworkNode(cx + 50, cy - 60, '#ec4899', 'LSTM'));
        this.nodes.push(new NetworkNode(cx + 50, cy, '#ec4899', 'LSTM'));
        this.nodes.push(new NetworkNode(cx + 50, cy + 60, '#ec4899', 'LSTM'));
        for (let i = 0; i < 5; i++) {
            this.nodes.push(new NetworkNode(cx + 200, cy - 80 + i * 40, '#10b981', 'Output'));
        }
        for (let i = 0; i < 8; i++) {
            for (let j = 8; j < 11; j++) this.connections.push(new Connection(i, j));
        }
        for (let j = 8; j < 11; j++) {
            for (let k = 11; k < 14; k++) this.connections.push(new Connection(j, k));
        }
        for (let k = 11; k < 14; k++) {
            for (let l = 14; l < 19; l++) this.connections.push(new Connection(k, l));
        }
    }
    animate() {
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        this.connections.forEach(c => { c.update(); c.draw(this.ctx, this.nodes); });
        this.nodes.forEach(n => n.draw(this.ctx));
        requestAnimationFrame(() => this.animate());
    }
}
class NetworkNode {
    constructor(x, y, color, label) {
        this.x = x; this.y = y; this.color = color; this.label = label; this.pulse = 0;
    }
    draw(ctx) {
        this.pulse += 0.05;
        const r = 15 + Math.sin(this.pulse) * 3;
        const g = ctx.createRadialGradient(this.x, this.y, 0, this.x, this.y, r * 2);
        g.addColorStop(0, this.color); g.addColorStop(1, 'transparent');
        ctx.fillStyle = g;
        ctx.beginPath(); ctx.arc(this.x, this.y, r * 2, 0, Math.PI * 2); ctx.fill();
        ctx.fillStyle = this.color;
        ctx.beginPath(); ctx.arc(this.x, this.y, r, 0, Math.PI * 2); ctx.fill();
        ctx.fillStyle = '#f1f5f9';
        ctx.font = '10px Inter';
        ctx.textAlign = 'center';
        ctx.fillText(this.label, this.x, this.y + 30);
    }
}
class Connection {
    constructor(from, to) {
        this.from = from; this.to = to; this.pulse = Math.random() * Math.PI * 2;
    }
    update() { this.pulse += 0.02; }
    draw(ctx, nodes) {
        const f = nodes[this.from], t = nodes[this.to];
        ctx.strokeStyle = `rgba(99, 102, 241, ${0.3 + Math.sin(this.pulse) * 0.2})`;
        ctx.lineWidth = 1;
        ctx.beginPath(); ctx.moveTo(f.x, f.y); ctx.lineTo(t.x, t.y); ctx.stroke();
    }
}
class ArchitectureDiagram {
    constructor(canvasId) {
        this.canvas = document.getElementById(canvasId);
        if (!this.canvas) return;
        this.ctx = this.canvas.getContext('2d');
        this.draw();
    }
    draw() {
        const ctx = this.ctx, w = this.canvas.width, h = this.canvas.height;
        ctx.fillStyle = '#1e293b'; ctx.fillRect(0, 0, w, h);
        this.drawComponent(100, 100, 150, 80, '#6366f1', 'Input Layer');
        this.drawComponent(300, 100, 150, 80, '#8b5cf6', 'Bayesian Layer');
        this.drawComponent(500, 100, 150, 80, '#ec4899', 'LSTM Layer');
        this.drawComponent(700, 100, 150, 80, '#10b981', 'Output Layer');
        ctx.strokeStyle = 'rgba(99, 102, 241, 0.5)'; ctx.lineWidth = 3;
        [[250,300],[450,500],[650,700]].forEach(([x1,x2]) => {
            ctx.beginPath(); ctx.moveTo(x1, 140); ctx.lineTo(x2, 140); ctx.stroke();
        });
        this.drawComponent(100, 250, 120, 60, '#f59e0b', 'Curriculum');
        this.drawComponent(300, 250, 120, 60, '#06b6d4', 'Spaced Rep.');
        this.drawComponent(500, 250, 120, 60, '#ef4444', 'Pavlovian');
        ctx.strokeStyle = 'rgba(139, 92, 246, 0.3)'; ctx.setLineDash([5, 5]);
        [[160,300],[360,500],[560,700]].forEach(([x1,x2]) => {
            ctx.beginPath(); ctx.moveTo(x1, 250); ctx.lineTo(x2, 180); ctx.stroke();
        });
        ctx.setLineDash([]);
    }
    drawComponent(x, y, w, h, color, label) {
        const ctx = this.ctx;
        ctx.fillStyle = 'rgba(0, 0, 0, 0.3)'; ctx.fillRect(x + 5, y + 5, w, h);
        ctx.fillStyle = color; ctx.fillRect(x, y, w, h);
        ctx.strokeStyle = '#f1f5f9'; ctx.lineWidth = 2; ctx.strokeRect(x, y, w, h);
        ctx.fillStyle = '#f1f5f9'; ctx.font = 'bold 16px Inter';
        ctx.textAlign = 'center'; ctx.fillText(label, x + w / 2, y + h / 2 + 5);
    }
}
document.addEventListener('DOMContentLoaded', () => {
    new BackgroundCanvas('backgroundCanvas');
    new NetworkVisualization('networkCanvas');
    new ArchitectureDiagram('architectureCanvas');
    document.querySelectorAll('a[href^="#"]').forEach(a => {
        a.addEventListener('click', function(e) {
            e.preventDefault();
            const t = document.querySelector(this.getAttribute('href'));
            if (t) t.scrollIntoView({behavior: 'smooth', block: 'start'});
        });
    });
    const obs = new IntersectionObserver(es => {
        es.forEach(e => { if (e.isIntersecting) e.target.classList.add('aos-animate'); });
    }, {threshold: 0.1});
    document.querySelectorAll('[data-aos]').forEach(el => obs.observe(el));
    const mt = document.getElementById('menuToggle');
    const nl = document.querySelector('.nav-links');
    if (mt && nl) mt.addEventListener('click', () => nl.classList.toggle('active'));
});
