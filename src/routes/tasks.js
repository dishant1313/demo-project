const express = require('express');
const { authenticateToken } = require('../middleware/auth');

const router = express.Router();

let tasks = [
  { id: 1, title: 'Setup project structure', description: 'Initialize repo with Express and Prisma', status: 'done', priority: 'high', assignee: 'dishant', sprintId: 1, storyPoints: 3, createdAt: '2026-04-15' },
  { id: 2, title: 'Implement user authentication', description: 'JWT-based auth with register and login', status: 'done', priority: 'high', assignee: 'dishant', sprintId: 1, storyPoints: 5, createdAt: '2026-04-16' },
  { id: 3, title: 'Create task CRUD endpoints', description: 'REST API for task management', status: 'in_progress', priority: 'high', assignee: 'dishant', sprintId: 1, storyPoints: 5, createdAt: '2026-04-17' },
  { id: 4, title: 'Add drag-and-drop kanban board', description: 'React DnD for task board UI', status: 'in_progress', priority: 'medium', assignee: 'arjun', sprintId: 2, storyPoints: 8, createdAt: '2026-04-20' },
  { id: 5, title: 'Sprint analytics dashboard', description: 'Charts for velocity and burndown', status: 'todo', priority: 'medium', assignee: 'priya', sprintId: 2, storyPoints: 5, createdAt: '2026-04-22' },
  { id: 6, title: 'Notification system', description: 'Email and in-app notifications for task updates', status: 'todo', priority: 'low', assignee: 'arjun', sprintId: 3, storyPoints: 8, createdAt: '2026-04-25' },
  { id: 7, title: 'AI task estimation', description: 'Integrate Gemini API for automatic story point estimation', status: 'todo', priority: 'high', assignee: 'dishant', sprintId: 3, storyPoints: 13, createdAt: '2026-04-28' },
];

let nextId = 8;

router.get('/', authenticateToken, (req, res) => {
  const { status, sprintId, assignee } = req.query;
  let filtered = [...tasks];
  if (status) filtered = filtered.filter(t => t.status === status);
  if (sprintId) filtered = filtered.filter(t => t.sprintId === parseInt(sprintId));
  if (assignee) filtered = filtered.filter(t => t.assignee === assignee);
  res.json(filtered);
});

router.get('/:id', authenticateToken, (req, res) => {
  const task = tasks.find(t => t.id === parseInt(req.params.id));
  if (!task) return res.status(404).json({ error: 'Task not found' });
  res.json(task);
});

router.post('/', authenticateToken, (req, res) => {
  const { title, description, priority, assignee, sprintId, storyPoints } = req.body;
  if (!title) return res.status(400).json({ error: 'Title is required' });
  
  const task = {
    id: nextId++,
    title,
    description: description || '',
    status: 'todo',
    priority: priority || 'medium',
    assignee: assignee || req.user.email,
    sprintId: sprintId || null,
    storyPoints: storyPoints || 0,
    createdAt: new Date().toISOString().split('T')[0]
  };
  tasks.push(task);
  res.status(201).json(task);
});

router.patch('/:id', authenticateToken, (req, res) => {
  const task = tasks.find(t => t.id === parseInt(req.params.id));
  if (!task) return res.status(404).json({ error: 'Task not found' });
  
  Object.assign(task, req.body);
  res.json(task);
});

router.delete('/:id', authenticateToken, (req, res) => {
  const index = tasks.findIndex(t => t.id === parseInt(req.params.id));
  if (index === -1) return res.status(404).json({ error: 'Task not found' });
  tasks.splice(index, 1);
  res.json({ message: 'Task deleted' });
});

module.exports = router;
