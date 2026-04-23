const express = require('express');
const { authenticateToken } = require('../middleware/auth');

const router = express.Router();

const sprints = [
  { id: 1, name: 'Sprint 1 - Foundation', goal: 'Setup core infrastructure and auth', startDate: '2026-04-15', endDate: '2026-04-28', status: 'completed', velocity: 13 },
  { id: 2, name: 'Sprint 2 - Core Features', goal: 'Build kanban board and analytics', startDate: '2026-04-29', endDate: '2026-05-12', status: 'active', velocity: 0 },
  { id: 3, name: 'Sprint 3 - AI Integration', goal: 'Add AI estimation and notifications', startDate: '2026-05-13', endDate: '2026-05-26', status: 'planned', velocity: 0 },
];

router.get('/', authenticateToken, (req, res) => {
  res.json(sprints);
});

router.get('/:id', authenticateToken, (req, res) => {
  const sprint = sprints.find(s => s.id === parseInt(req.params.id));
  if (!sprint) return res.status(404).json({ error: 'Sprint not found' });
  res.json(sprint);
});

router.get('/:id/burndown', authenticateToken, (req, res) => {
  // Simulated burndown data
  const sprint = sprints.find(s => s.id === parseInt(req.params.id));
  if (!sprint) return res.status(404).json({ error: 'Sprint not found' });

  const burndown = [
    { day: 1, ideal: 13, actual: 13 },
    { day: 2, ideal: 12, actual: 13 },
    { day: 3, ideal: 11, actual: 11 },
    { day: 4, ideal: 10, actual: 10 },
    { day: 5, ideal: 9, actual: 9 },
    { day: 6, ideal: 8, actual: 8 },
    { day: 7, ideal: 6, actual: 7 },
    { day: 8, ideal: 5, actual: 5 },
    { day: 9, ideal: 4, actual: 3 },
    { day: 10, ideal: 2, actual: 2 },
    { day: 11, ideal: 1, actual: 1 },
    { day: 12, ideal: 0, actual: 0 },
  ];

  res.json({ sprint: sprint.name, data: burndown });
});

module.exports = router;
