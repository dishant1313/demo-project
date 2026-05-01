const { authenticateToken } = require('../middleware/auth');
const { generateToken } = require('../middleware/auth');

describe('Auth Middleware', () => {
  test('should reject request without token', () => {
    const req = { headers: {} };
    const res = { status: jest.fn().mockReturnThis(), json: jest.fn() };
    const next = jest.fn();

    authenticateToken(req, res, next);

    expect(res.status).toHaveBeenCalledWith(401);
    expect(next).not.toHaveBeenCalled();
  });

  test('should generate valid JWT token', () => {
    const token = generateToken({ id: 1, email: 'test@test.com' });
    expect(token).toBeDefined();
    expect(typeof token).toBe('string');
    expect(token.split('.')).toHaveLength(3);
  });
});
