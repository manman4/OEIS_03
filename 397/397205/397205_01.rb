# A397205: max |S|*|T| over S,T ⊆ {1..n} with all products s*t distinct.
#
#   s1*t1 = s2*t2  <=>  s1/s2 = t2/t1
#   よって条件は R(S) ∩ R(T) = ∅ と同値（R(X) = X 内の既約比の集合）。

POP16 = Array.new(1 << 16) { |i| i.to_s(2).count("1") }

class Solver
  def initialize(n)
    @n = n
    @full = ((1 << (n + 1)) - 1) & ~1            # ビット 1..n
    @rid = Array.new(n + 1) { Array.new(n + 1, -1) }   # ペア -> 比クラスID
    @pairs = []                                        # 比クラスID -> そのペア一覧
    ids = {}
    (2..n).each do |j|
      (1...j).each do |i|
        g = i.gcd(j)
        id = ids[[i / g, j / g]] ||= (@pairs << []; @pairs.size - 1)
        @rid[i][j] = @rid[j][i] = id
        @pairs[id] << [i, j]
      end
    end
  end

  # 貪欲彩色による最大クリークの上界
  def color_bound(adj, cand)
    c = 0
    un = cand
    while un != 0
      c += 1
      q = un
      while q != 0
        b = q & -q
        v = b.bit_length - 1
        un &= ~b
        q &= ~b
        q &= ~adj[v]                              # 同色は独立集合
      end
    end
    c
  end

  # Tomita 流の探索: cand 内にサイズ need のクリークがあれば返す、なければ nil
  def expand(adj, cand, cur, need)
    order = []
    cols  = []
    un = cand
    c = 0
    while un != 0
      c += 1
      q = un
      while q != 0
        b = q & -q
        v = b.bit_length - 1
        un &= ~b
        q &= ~b
        q &= ~adj[v]
        order << v
        cols  << c
      end
    end
    i = order.size - 1
    while i >= 0
      return nil if cur.size + cols[i] < need      # 彩色数による枝刈り
      v = order[i]
      cur.push(v)
      if cur.size == need
        r = cur.dup
        cur.pop
        return r
      end
      r = expand(adj, cand & adj[v], cur, need)
      cur.pop
      return r if r
      cand &= ~(1 << v)
      i -= 1
    end
    nil
  end

  def solve
    @best  = @n
    @bestS = [1]
    @bestT = (1..@n).to_a
    adj = Array.new(@n + 1, 0)
    (1..@n).each { |v| adj[v] = @full & ~(1 << v) }
    dfs([], 0, adj, 1)
    [@best, @bestS, @bestT]
  end

  private

  # S を昇順に伸ばす DFS。adj は「R(S) に属さない比のペア」だけを残したグラフ
  def dfs(s, rmask, adj, start)
    size = s.size
    if size > 0
      ub = color_bound(adj, @full)
      return if ub < size                                    # |T| >= |S| が以後不可能
      return if [size + (@n - start + 1), ub].min * ub <= @best
      if s.inject(0) { |a, b| a.gcd(b) } == 1                # R(cS)=R(S) なので gcd(S)=1 のみ評価
        need = [size, @best / size + 1].max
        while (cl = expand(adj, @full, [], need))
          if size * cl.size > @best
            @best  = size * cl.size
            @bestS = s.dup
            @bestT = cl.sort
          end
          need += 1
        end
      end
    end
    v = start
    while v <= @n
      radj = adj
      rm = rmask
      copied = false
      s.each do |u|
        id = @rid[u][v]
        next if rm[id] == 1
        rm |= 1 << id
        (radj = adj.dup; copied = true) unless copied
        @pairs[id].each { |i, j| radj[i] &= ~(1 << j); radj[j] &= ~(1 << i) }
      end
      s.push(v)
      dfs(s, rm, radj, v + 1)
      s.pop
      v += 1
    end
  end
end

def check(n, s, t)
  p = s.product(t).map { |a, b| a * b }
  p.uniq.size == p.size && (s + t).max <= n
end

if __FILE__ == $0
  lo = (ARGV[0] || 1).to_i
  hi = (ARGV[1] || 24).to_i
  (lo..hi).each do |n|
    t0 = Process.clock_gettime(Process::CLOCK_MONOTONIC)
    v, s, t = Solver.new(n).solve
    raise "bad #{n}" unless check(n, s, t)
    printf("a(%d) = %d\t[%.2fs]  S=%p T=%p\n",
           n, v, Process.clock_gettime(Process::CLOCK_MONOTONIC) - t0, s, t)
    $stdout.flush
  end
end