class Fenwick
  def initialize(n)
    @n = n
    @bit = Array.new(n + 1, 0)
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

class A304405DP
  def initialize(max_n)
    @max_n = max_n
    @divisors = Array.new(max_n + 1) { [] }
    build_divisors
  end

  # a(n) = f(n, n+1, n+1)
  # f(n, S, D) = Σ_{s<=min(n,S)} Σ_{d|s, d<D} f(n-s, s, d)
  #
  # 固定 n で s を小さい順に処理し、d 軸を Fenwick で管理する。
  # - 点 (d) に重みを加える: 選べる新しいグループ (s, d)
  # - クエリ: f(n, s, d_limit) = Σ_{d<d_limit} weight(d) = bit.sum(d_limit-1)
  def each_value
    return enum_for(:each_value) unless block_given?

    yield 0, 1

    # s<n の状態は d_limit が常に s の約数になるので、
    # 約数配列と同じ添字順で保持する。
    # dp_sparse[n][s][i] = f(n, s, divisors[s][i])
    dp_sparse = Array.new(@max_n + 1) { [] }
    # s=n の状態だけは d_limit が任意になり得るため配列で保持
    # dp_full[n][d_limit] = f(n, n, d_limit), d_limit: 1..n+1
    dp_full = Array.new(@max_n + 1)

    n = 1
    while n <= @max_n
      bit = Fenwick.new(n)
      active_total = 0
      sparse_rows = Array.new(n + 1)

      s = 1
      while s <= n
        active_total += add_points_for_s(bit, n, s, dp_sparse, dp_full)
        ds = @divisors[s]

        if s < n
          row = Array.new(ds.length, 0)
          j = 0
          while j < ds.length
            d_limit = ds[j]
            row[j] = bit.sum(d_limit - 1)
            j += 1
          end
          sparse_rows[s] = row
        else
          full = Array.new(n + 2, 0)
          d_limit = 1
          while d_limit <= n
            full[d_limit] = bit.sum(d_limit - 1)
            d_limit += 1
          end
          full[n + 1] = active_total
          dp_full[n] = full
          yield n, full[n + 1]
        end
        s += 1
      end

      dp_sparse[n] = sparse_rows
      n += 1
    end
  end

  def values
    ary = Array.new(@max_n + 1)
    each_value { |i, v| ary[i] = v }
    ary
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

  def add_points_for_s(bit, n, s, dp_sparse, dp_full)
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

    if s < n2
      prev = dp_sparse[n2][s]
      i = 0
      while i < ds.length
        w = prev ? prev[i] : 0
        if w != 0
          bit.add(ds[i], w)
          total_added += w
        end
        i += 1
      end
      return total_added
    end

    i = 0
    while i < ds.length
      d = ds[i]
      d2 = d <= n2 ? d : (n2 + 1)
      w = dp_full[n2][d2]
      if w != 0
        bit.add(d, w)
        total_added += w
      end
      i += 1
    end
    total_added
  end
end


# m>0
def s(f_ary, g_ary, n, m)
  s = 0
  (m..n).each{|i| s += i * f_ary[i] * g_ary[i] ** (n / i) if n % i == 0}
  s
end

def A(ary, n, m = 1)
  a_ary = [1]
  a = [0] + (1..n).map{|i| ary.inject(0){|s, j| s + s(j[0], j[1], i, m)}}
  (1..n).each{|i| a_ary << (1..i).inject(0){|s, j| s + a[j] * a_ary[-j]} / i}
  a_ary
end

n = 5000
digit_limit = 1000

ary1 = Array.new(n + 1, 1)
# Product_{k>0} 1/(1 - x^k)
p_ary = A([[ary1, ary1]], n)

solver = A304405DP.new(n)
solver.each_value{|i, v|
  j = p_ary[i] - v
  break if j.to_s.size > digit_limit
  print i
  print ' '
  puts j
}
