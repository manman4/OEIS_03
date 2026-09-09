# Erdos problem #896
#   F(A,B) = #{ m : m = a*b has exactly one solution with a in A, b in B }
#   compute  max F(A,B)  over A,B subseteq {1..N}
require 'set'

class Solver896
  def initialize(n)
    @n = n
    @mm = n * n
    @cnt = Array.new(@mm + 1, 0)      # committed representations of m
    @pos = Array.new(@mm + 1, 0)      # still-possible representations of m
    (1..n).each { |a| (1..n).each { |b| @pos[a * b] += 1 } }
    @f = 0                            # #{m : cnt[m] == 1}  (value of current A0,B0)
    @lc = 0                           # sum of cnt[m] over m with cnt[m] >= 2
    @ub2 = 0
    (1..@mm).each { |m| @ub2 += u(m) }
    @A = []
    @B = []
    @log = []
    @best = 0
    @bestA = []
    @bestB = []
    @nodes = 0
  end

  def u(m)
    c = @cnt[m]
    (c == 1 || (c == 0 && @pos[m] > 0)) ? 1 : 0
  end

  def inc_cnt(m)
    @ub2 -= u(m)
    c = @cnt[m]
    @f -= 1 if c == 1
    @lc += (c == 1 ? 2 : (c >= 2 ? 1 : 0))
    @cnt[m] = c + 1
    @f += 1 if c + 1 == 1
    @ub2 += u(m)
    @log << m
  end

  def dec_cnt(m)
    @ub2 -= u(m)
    c = @cnt[m]
    @f -= 1 if c == 1
    @lc -= (c == 2 ? 2 : (c > 2 ? 1 : 0))
    @cnt[m] = c - 1
    @f += 1 if c - 1 == 1
    @ub2 += u(m)
  end

  def dec_pos(m)
    @ub2 -= u(m)
    @pos[m] -= 1
    @ub2 += u(m)
    @log << (-m)
  end

  def inc_pos(m)
    @ub2 -= u(m)
    @pos[m] += 1
    @ub2 += u(m)
  end

  def solve
    dfs(1, 0)
    [@best, @bestA, @bestB, @nodes]
  end

  private

  def dfs(v, sym)
    @nodes += 1
    if @f > @best
      @best = @f
      @bestA = @A.dup
      @bestB = @B.dup
    end
    return if v > @n
    free = @n - v + 1
    ub = (@A.size + free) * (@B.size + free) - @lc
    ub = @ub2 if @ub2 < ub
    return if ub <= @best

    [[1, 1], [1, 0], [0, 1], [0, 0]].each do |ca, cb|
      next if sym == 0 && ca == 0 && cb == 1        # F(A,B) = F(B,A)
      mark = @log.size
      apply(v, ca, cb)
      dfs(v + 1, (ca == cb) ? sym : 1)
      @A.pop if ca == 1
      @B.pop if cb == 1
      undo(mark)
    end
  end

  def apply(v, ca, cb)
    @A.each { |a| inc_cnt(a * v) } if cb == 1       # A0 x {v}
    if ca == 1
      @B.each { |b| inc_cnt(v * b) }                # {v} x B0
      inc_cnt(v * v) if cb == 1
    end
    if ca == 0                                      # v can never be in A
      @B.each { |b| dec_pos(v * b) }
      (v..@n).each { |b| dec_pos(v * b) }
    end
    if cb == 0                                      # v can never be in B
      @A.each { |a| dec_pos(a * v) }
      dec_pos(v * v) if ca == 1
      ((v + 1)..@n).each { |a| dec_pos(a * v) }
    end
    @A << v if ca == 1
    @B << v if cb == 1
  end

  def undo(mark)
    while @log.size > mark
      m = @log.pop
      if m > 0 then dec_cnt(m) else inc_pos(-m) end
    end
  end
end

def eval_F(n, a, b)
  c = Hash.new(0)
  a.each { |x| b.each { |y| c[x * y] += 1 } }
  c.count { |_, v| v == 1 }
end

if __FILE__ == $0
  lo = (ARGV[0] || 1).to_i
  hi = (ARGV[1] || 14).to_i
  (lo..hi).each do |n|
    t0 = Process.clock_gettime(Process::CLOCK_MONOTONIC)
    f, a, b, nodes = Solver896.new(n).solve
    raise "bad #{n}" unless eval_F(n, a, b) == f
    printf("N=%2d  maxF=%4d  [%8.2fs %10d nodes]  A=%p B=%p\n",
           n, f, Process.clock_gettime(Process::CLOCK_MONOTONIC) - t0, nodes, a, b)
    $stdout.flush
  end
end
