@max_n = -1
@comb = []
@euler = []
@d_table = []

def ensure_tables(n_max)
  return if n_max <= @max_n

  @comb = Array.new(n_max + 1){Array.new(n_max + 1, 0)}
  0.upto(n_max){|n|
    @comb[n][0] = 1
    @comb[n][n] = 1
    1.upto(n - 1){|k|
      @comb[n][k] = @comb[n - 1][k - 1] + @comb[n - 1][k]
    }
  }

  @euler = Array.new(n_max + 1){Array.new(n_max + 1, 0)}
  @euler[0][0] = 1
  1.upto(n_max){|n|
    0.upto(n - 1){|k|
      @euler[n][k] =
        (k > 0 ? (n - k) * @euler[n - 1][k - 1] : 0) +
        (k + 1) * @euler[n - 1][k]
    }
  }

  # d(n,k): derangement of [n] with exactly k excedances.
  @d_table = Array.new(n_max + 1){Array.new(n_max + 1, 0)}
  0.upto(n_max){|n|
    0.upto(n){|k|
      s = 0
      0.upto(n){|j|
        term = @comb[n][j] * @euler[n - j][k]
        s += j.even? ? term : -term
      }
      @d_table[n][k] = s
    }
  }

  @max_n = n_max
end

# A(n, k): number of permutations of [n] with (#moved_left - #moved_right) = k.
def a_excess(n, k)
  return 0 if n < 0

  ensure_tables(n)
  s = 0
  m = k.abs
  m += 1 if (m + k).odd?
  while m <= n
    e = (m + k) / 2
    s += @comb[n][m] * @d_table[m][e]
    m += 2
  end
  s
end

# Requested sequence:
# number of permutations of [2*n] with left-right excess equal to n.
def a(n)
  a_excess(2 * n, n)
end

def a_slow(n)
  m = 2 * n
  base = (1..m).to_a
  base.permutation.count{|perm|
    moved_left = 0
    moved_right = 0
    perm.each_with_index{|value, idx|
      pos = idx + 1
      moved_left += 1 if pos < value
      moved_right += 1 if pos > value
    }
    moved_left - moved_right == n
  }
end

n_max = 20
if ENV["VERIFY"] == "1"
  0.upto([n_max, 4].min){|n|
    fast = a(n)
    slow = a_slow(n)
    raise "mismatch n=#{n}: fast=#{fast}, slow=#{slow}" if fast != slow
  }
end
b=[]
0.upto(n_max){|n|
  b << a(n)
  puts "#{n} #{a(n)}"
}
p b
