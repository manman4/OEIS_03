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
    return 0 if i <= 0
    i = @n if i > @n
    s = 0
    while i > 0
      s += @bit[i]
      i -= i & -i
    end
    s
  end
end

# a(n) = f(n, n+1, 0)
# f(n, S, D) = Σ_{s < S, s<=n} Σ_{d|s, d>D} f(n-s, s, d)
class A304430DP
  def initialize(max_n)
    @max_n = max_n
    @divisors = Array.new(max_n + 1) { [] }
    build_divisors
  end

  def each_value
    return enum_for(:each_value) unless block_given?

    yield 0, 1

    # dp_sparse[n][s][i] = f(n, s, divisors[s][i]) for 1<=s<=n
    dp_sparse = Array.new(@max_n + 1) { [] }
    # dp_full[n][d] = f(n, n+1, d) for 1<=d<=n+1
    dp_full = Array.new(@max_n + 1)

    n = 1
    while n <= @max_n
      bit = Fenwick.new(n)
      active_total = 0
      sparse_rows = Array.new(n + 1)

      s_limit = 1
      while s_limit <= n + 1
        s_add = s_limit - 1
        if s_add >= 1
          active_total += add_points_for_s(bit, n, s_add, dp_sparse, dp_full)
        end

        if s_limit <= n
          ds = @divisors[s_limit]
          row = Array.new(ds.length, 0)
          i = 0
          while i < ds.length
            d_prev = ds[i]
            row[i] = active_total - bit.sum(d_prev)
            i += 1
          end
          sparse_rows[s_limit] = row
        else
          full = Array.new(n + 2, 0)
          d_prev = 1
          while d_prev <= n + 1
            full[d_prev] = active_total - bit.sum(d_prev)
            d_prev += 1
          end
          dp_full[n] = full

          # a(n) = f(n, n+1, 0) = Σ_{d>0} weight(d)
          yield n, active_total
        end

        s_limit += 1
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

  def add_points_for_s(bit, n, s_add, dp_sparse, dp_full)
    n2 = n - s_add
    ds = @divisors[s_add]
    total_added = 0

    i = 0
    while i < ds.length
      d = ds[i]
      weight =
        if n2 == 0
          1
        elsif s_add <= n2
          prev = dp_sparse[n2][s_add]
          prev ? prev[i] : 0
        else
          d2 = d <= n2 ? d : (n2 + 1)
          dp_full[n2][d2]
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

solver = A304430DP.new(n)
solver.each_value{|i, v|
  break if v.to_s.size > digit_limit
  print i
  print ' '
  puts v
}
