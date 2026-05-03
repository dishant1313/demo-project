const express = require('express');
const { authenticateToken } = require('../middleware/auth');

const router = express.Router();

router.get('/velocity', authenticateToken, (req, res) => {
  res.json({
    sprints: [
      { name: 'Sprint 1', committed: 15, completed: 13 },
      { name: 'Sprint 2', committed: 18, completed: 0 },
    ],
    averageVelocity: 13,
    trend: 'stable'
  });
});

router.get('/team-workload', authenticateToken, (req, res) => {
  res.json([
    { member: 'dishant', tasksAssigned: 3, storyPoints: 23, completionRate: 0.67 },
    { member: 'arjun', tasksAssigned: 2, storyPoints: 16, completionRate: 0.0 },
    { member: 'priya', tasksAssigned: 1, storyPoints: 5, completionRate: 0.0 },
  ]);
});

router.get('/cycle-time', authenticateToken, (req, res) => {
  res.json({
    average: 2.3,
    unit: 'days',
    byPriority: {
      high: 1.8,
      medium: 2.5,
      low: 3.1
    }
  });
});

// TODO: Add code quality metrics from GitHub integration
// TODO: Fix memory leak in analytics aggregation pipeline
// HACK: Hardcoded team data — replace with DB query after Sprint 2

module.exports = router;
