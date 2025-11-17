"use strict";
// Copyright (C) 2025, Shyamal Suhana Chandra
// All rights reserved.
// Background Canvas Animation
class BackgroundCanvas {
    constructor(canvasId) {
        this.canvas = null;
        this.ctx = null;
        this.particles = [];
        this.animationId = 0;
        this.canvas = document.getElementById(canvasId);
        if (!this.canvas)
            return;
        const context = this.canvas.getContext('2d');
        if (!context)
            return;
        this.ctx = context;
        this.resize();
        this.initParticles();
        this.animate();
        window.addEventListener('resize', () => this.resize());
    }
    resize() {
        if (!this.canvas)
            return;
        this.canvas.width = window.innerWidth;
        this.canvas.height = window.innerHeight;
    }
    initParticles() {
        if (!this.canvas)
            return;
        const count = Math.floor((this.canvas.width * this.canvas.height) / 15000);
        this.particles = [];
        for (let i = 0; i < count; i++) {
            this.particles.push(new Particle(Math.random() * this.canvas.width, Math.random() * this.canvas.height, this.canvas.width, this.canvas.height));
        }
    }
    animate() {
        if (!this.ctx || !this.canvas)
            return;
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        this.particles.forEach(particle => {
            particle.update();
            particle.draw(this.ctx);
        });
        // Draw connections
        this.drawConnections();
        this.animationId = requestAnimationFrame(() => this.animate());
    }
    drawConnections() {
        if (!this.ctx)
            return;
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
        this.x = x;
        this.y = y;
        this.vx = (Math.random() - 0.5) * 0.5;
        this.vy = (Math.random() - 0.5) * 0.5;
        this.radius = Math.random() * 2 + 1;
        this.maxX = maxX;
        this.maxY = maxY;
    }
    update() {
        this.x += this.vx;
        this.y += this.vy;
        if (this.x < 0 || this.x > this.maxX)
            this.vx *= -1;
        if (this.y < 0 || this.y > this.maxY)
            this.vy *= -1;
    }
    draw(ctx) {
        ctx.fillStyle = 'rgba(99, 102, 241, 0.5)';
        ctx.beginPath();
        ctx.arc(this.x, this.y, this.radius, 0, Math.PI * 2);
        ctx.fill();
    }
}
// Network Visualization Canvas
class NetworkVisualization {
    constructor(canvasId) {
        this.canvas = null;
        this.ctx = null;
        this.nodes = [];
        this.connections = [];
        this.animationId = 0;
        this.canvas = document.getElementById(canvasId);
        if (!this.canvas)
            return;
        const context = this.canvas.getContext('2d');
        if (!context)
            return;
        this.ctx = context;
        this.initNetwork();
        this.animate();
    }
    initNetwork() {
        if (!this.canvas)
            return;
        const centerX = this.canvas.width / 2;
        const centerY = this.canvas.height / 2;
        // Input layer
        for (let i = 0; i < 8; i++) {
            this.nodes.push(new NetworkNode(centerX - 200, centerY - 120 + i * 30, '#6366f1', 'Input'));
        }
        // Bayesian layer
        this.nodes.push(new NetworkNode(centerX - 50, centerY - 60, '#8b5cf6', 'Bayesian'));
        this.nodes.push(new NetworkNode(centerX - 50, centerY, '#8b5cf6', 'Bayesian'));
        this.nodes.push(new NetworkNode(centerX - 50, centerY + 60, '#8b5cf6', 'Bayesian'));
        // LSTM layer
        this.nodes.push(new NetworkNode(centerX + 50, centerY - 60, '#ec4899', 'LSTM'));
        this.nodes.push(new NetworkNode(centerX + 50, centerY, '#ec4899', 'LSTM'));
        this.nodes.push(new NetworkNode(centerX + 50, centerY + 60, '#ec4899', 'LSTM'));
        // Output layer
        for (let i = 0; i < 5; i++) {
            this.nodes.push(new NetworkNode(centerX + 200, centerY - 80 + i * 40, '#10b981', 'Output'));
        }
        // Create connections
        for (let i = 0; i < 8; i++) {
            for (let j = 8; j < 11; j++) {
                this.connections.push(new Connection(i, j));
            }
        }
        for (let j = 8; j < 11; j++) {
            for (let k = 11; k < 14; k++) {
                this.connections.push(new Connection(j, k));
            }
        }
        for (let k = 11; k < 14; k++) {
            for (let l = 14; l < 19; l++) {
                this.connections.push(new Connection(k, l));
            }
        }
    }
    animate() {
        if (!this.ctx || !this.canvas)
            return;
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        // Draw connections
        this.connections.forEach(conn => {
            conn.update();
            conn.draw(this.ctx, this.nodes);
        });
        // Draw nodes
        this.nodes.forEach(node => node.draw(this.ctx));
        this.animationId = requestAnimationFrame(() => this.animate());
    }
}
class NetworkNode {
    constructor(x, y, color, label) {
        this.pulse = 0;
        this.x = x;
        this.y = y;
        this.color = color;
        this.label = label;
    }
    draw(ctx) {
        this.pulse += 0.05;
        const radius = 15 + Math.sin(this.pulse) * 3;
        // Glow effect
        const gradient = ctx.createRadialGradient(this.x, this.y, 0, this.x, this.y, radius * 2);
        gradient.addColorStop(0, this.color);
        gradient.addColorStop(1, 'transparent');
        ctx.fillStyle = gradient;
        ctx.beginPath();
        ctx.arc(this.x, this.y, radius * 2, 0, Math.PI * 2);
        ctx.fill();
        // Node
        ctx.fillStyle = this.color;
        ctx.beginPath();
        ctx.arc(this.x, this.y, radius, 0, Math.PI * 2);
        ctx.fill();
        // Label
        ctx.fillStyle = '#f1f5f9';
        ctx.font = '10px Inter';
        ctx.textAlign = 'center';
        ctx.fillText(this.label, this.x, this.y + 30);
    }
}
class Connection {
    constructor(from, to) {
        this.pulse = Math.random() * Math.PI * 2;
        this.from = from;
        this.to = to;
    }
    update() {
        this.pulse += 0.02;
    }
    draw(ctx, nodes) {
        const fromNode = nodes[this.from];
        const toNode = nodes[this.to];
        const alpha = 0.3 + Math.sin(this.pulse) * 0.2;
        ctx.strokeStyle = `rgba(99, 102, 241, ${alpha})`;
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.moveTo(fromNode.x, fromNode.y);
        ctx.lineTo(toNode.x, toNode.y);
        ctx.stroke();
    }
}
// Architecture Diagram
class ArchitectureDiagram {
    constructor(canvasId) {
        this.canvas = null;
        this.ctx = null;
        this.canvas = document.getElementById(canvasId);
        if (!this.canvas)
            return;
        const context = this.canvas.getContext('2d');
        if (!context)
            return;
        this.ctx = context;
        this.draw();
    }
    draw() {
        if (!this.ctx || !this.canvas)
            return;
        const ctx = this.ctx;
        const width = this.canvas.width;
        const height = this.canvas.height;
        // Clear
        ctx.fillStyle = '#1e293b';
        ctx.fillRect(0, 0, width, height);
        // Draw components
        this.drawComponent(100, 100, 150, 80, '#6366f1', 'Input Layer');
        this.drawComponent(300, 100, 150, 80, '#8b5cf6', 'Bayesian Layer');
        this.drawComponent(500, 100, 150, 80, '#ec4899', 'LSTM Layer');
        this.drawComponent(700, 100, 150, 80, '#10b981', 'Output Layer');
        // Draw connections
        ctx.strokeStyle = 'rgba(99, 102, 241, 0.5)';
        ctx.lineWidth = 3;
        ctx.beginPath();
        ctx.moveTo(250, 140);
        ctx.lineTo(300, 140);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(450, 140);
        ctx.lineTo(500, 140);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(650, 140);
        ctx.lineTo(700, 140);
        ctx.stroke();
        // Draw side components
        this.drawComponent(100, 250, 120, 60, '#f59e0b', 'Curriculum');
        this.drawComponent(300, 250, 120, 60, '#06b6d4', 'Spaced Rep.');
        this.drawComponent(500, 250, 120, 60, '#ef4444', 'Pavlovian');
        // Draw connections to main flow
        ctx.strokeStyle = 'rgba(139, 92, 246, 0.3)';
        ctx.setLineDash([5, 5]);
        ctx.beginPath();
        ctx.moveTo(160, 250);
        ctx.lineTo(300, 180);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(360, 250);
        ctx.lineTo(500, 180);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(560, 250);
        ctx.lineTo(700, 180);
        ctx.stroke();
        ctx.setLineDash([]);
    }
    drawComponent(x, y, w, h, color, label) {
        if (!this.ctx)
            return;
        const ctx = this.ctx;
        // Shadow
        ctx.fillStyle = 'rgba(0, 0, 0, 0.3)';
        ctx.fillRect(x + 5, y + 5, w, h);
        // Component
        ctx.fillStyle = color;
        ctx.fillRect(x, y, w, h);
        // Border
        ctx.strokeStyle = '#f1f5f9';
        ctx.lineWidth = 2;
        ctx.strokeRect(x, y, w, h);
        // Label
        ctx.fillStyle = '#f1f5f9';
        ctx.font = 'bold 16px Inter';
        ctx.textAlign = 'center';
        ctx.fillText(label, x + w / 2, y + h / 2 + 5);
    }
}
// Smooth Scroll
function initSmoothScroll() {
    document.querySelectorAll('a[href^="#"]').forEach(anchor => {
        anchor.addEventListener('click', function (e) {
            e.preventDefault();
            const href = this.getAttribute('href');
            if (!href)
                return;
            const target = document.querySelector(href);
            if (target) {
                target.scrollIntoView({
                    behavior: 'smooth',
                    block: 'start'
                });
            }
        });
    });
}
// Intersection Observer for animations
function initScrollAnimations() {
    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.classList.add('aos-animate');
            }
        });
    }, {
        threshold: 0.1
    });
    document.querySelectorAll('[data-aos]').forEach(el => {
        observer.observe(el);
    });
}
// Mobile menu toggle
function initMobileMenu() {
    const menuToggle = document.getElementById('menuToggle');
    const navLinks = document.querySelector('.nav-links');
    if (menuToggle && navLinks) {
        menuToggle.addEventListener('click', () => {
            navLinks.classList.toggle('active');
        });
    }
}
// Initialize everything when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    new BackgroundCanvas('backgroundCanvas');
    new NetworkVisualization('networkCanvas');
    new ArchitectureDiagram('architectureCanvas');
    initSmoothScroll();
    initScrollAnimations();
    initMobileMenu();
});
//# sourceMappingURL=app.js.map