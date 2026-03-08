# a(n) = f(n, 0, 0)
# f(n, S, D) = sum_{s=S+1..n} sum_{d|s, d>D} f(n-s, s, d), f(0, *, *) = 1
#
# 計算方針:
# - n を小さい順に埋めるボトムアップDP
# - 固定 n で S を大きい方から走査し、
#   既に有効な点 (s, d) (s > S) の重みを d 軸Fenwick Treeで管理
# - f(n, S, D) = sum_{d>D} weight(d)
#   = active_total - prefix_sum(D)
#
# これにより、2次元面 O(n^2) 走査を避け、
# 1ステップを「有効点数 ~ n log n」に比例した処理にする。
class Fenwick
  def initialize(size)
    @n = size
    @bit = Array.new(size + 1, 0)
  end

  def add(i, delta)
    while i <= @n
      @bit[i] += delta
      i += i & -i
    end
  end

  def sum(i)
    s = 0
    while i > 0
      s += @bit[i]
      i -= i & -i
    end
    s
  end
end

class A304428DP
  def initialize(max_n)
    @max_n = max_n
    @divisors = Array.new(max_n + 1){[]}
    build_divisors

    # dp[n][s][k] = f(n, s, divisors[s][k])
    # s < n の範囲のみ保持し、d は d|s のみ保持（疎）。
    @dp = Array.new(max_n + 1) { [] }
  end

  def each_value
    return enum_for(:each_value) unless block_given?

    yield 0, 1

    n = 1
    while n <= @max_n
      yield n, build_n(n)
      n += 1
    end
  end

  private

  def build_divisors
    d = 1
    while d <= @max_n
      m = d
      while m <= @max_n
        @divisors[m] << d
        m += d
      end
      d += 1
    end
  end

  def add_points_for_s(bit, s, n)
    n2 = n - s
    ds = @divisors[s]
    total_added = 0

    if n2 == 0
      i = 0
      while i < ds.length
        d = ds[i]
        bit.add(d, 1)
        total_added += 1
        i += 1
      end
      return total_added
    end

    # f(n2, s, d) が定義されるのは s < n2 のときのみ。
    return 0 unless s < n2

    prev = @dp[n2][s]
    i = 0
    while i < ds.length
      w = prev[i]
      if w != 0
        bit.add(ds[i], w)
        total_added += w
      end
      i += 1
    end

    total_added
  end

  def build_n(n)
    bit = Fenwick.new(n)
    active_total = 0
    row = Array.new(n)

    s_prev = n - 1
    while s_prev >= 0
      s_add = s_prev + 1
      active_total += add_points_for_s(bit, s_add, n)

      if s_prev == 0
        # a(n) = f(n, 0, 0)
        a_n = active_total
      else
        ds = @divisors[s_prev]
        arr = Array.new(ds.length)
        i = 0
        while i < ds.length
          d_prev = ds[i]
          # sum_{d > d_prev}
          arr[i] = active_total - bit.sum(d_prev)
          i += 1
        end
        row[s_prev] = arr
      end

      s_prev -= 1
    end

    @dp[n] = row
    a_n
  end
end

n = 10000
digit_limit = 1000

solver = A304428DP.new(n)
solver.each_value{|i, v|
  break if v.to_s.size > digit_limit
  print i
  print ' '
  puts v
}
