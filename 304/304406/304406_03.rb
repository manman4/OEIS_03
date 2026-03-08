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

# a(n) = f(n, n, 1)
# f(n, S, D) = Σ_{s<=min(n,S)} Σ_{d|s, d>=D} f(n-s, s, d+1)
class A304406DP
  def initialize(max_n)
    @max_n = max_n
    @divisors = Array.new(max_n + 1) { [] }
    build_divisors
  end

  def each_value
    return enum_for(:each_value) unless block_given?

    yield 0, 1

    # s<n の状態は D が divisors[s][i] + 1 の形でのみ参照される
    # dp_sparse[n][s][i] = f(n, s, divisors[s][i] + 1)
    dp_sparse = Array.new(@max_n + 1) { [] }
    # s=n の状態は D が密になるので配列で保持
    # dp_full[n][d_min] = f(n, n, d_min), d_min: 1..n+1
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
          i = 0
          while i < ds.length
            d = ds[i]
            # D = d + 1 の suffix 和
            row[i] = active_total - bit.sum(d)
            i += 1
          end
          sparse_rows[s] = row
        else
          full = Array.new(n + 2, 0)
          d_min = 1
          while d_min <= n + 1
            full[d_min] = active_total - bit.sum(d_min - 1)
            d_min += 1
          end
          dp_full[n] = full
          # a(n) = f(n, n, 1)
          yield n, full[1]
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

    i = 0
    while i < ds.length
      d = ds[i]
      weight =
        if n2 == 0
          1
        elsif s < n2
          prev = dp_sparse[n2][s]
          prev ? prev[i] : 0
        else
          d2 = d + 1
          if d2 <= n2 + 1
            dp_full[n2][d2]
          else
            0
          end
        end

      if weight != 0
        bit.add(d, weight)
        total_added += weight
      end
      i += 1
    end

    total_added
  end
end

n = 5000
digit_limit = 1000

solver = A304406DP.new(n)
solver.each_value{|i, v|
  break if v.to_s.size > digit_limit
  print i
  print ' '
  puts v
}
