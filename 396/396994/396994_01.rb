# A[row][col] corresponds to a(row, col-1, l).

def antidiagonal_terms(row_num, l = 3)
  k_limit = Array.new(row_num + 1, 0)
  1.upto(row_num){|row|
    k_limit[row] = (row_num - row) * l
  }

  a = Array.new(row_num + 1){|row| Array.new(k_limit[row] + 1, 0)}

  0.upto(k_limit[1]){|col|
    a[1][col] = 1
  }

  2.upto(row_num){|row|
    a[row][0] = 0
  }

  2.upto(row_num){|row|
    1.upto(k_limit[row]){|col|
      total = a[row][col - 1]
      1.upto(row - 1){|j|
        total += a[j][col + l - 1] * a[row - j][col - 1]
      }
      a[row][col] = total
    }
  }

  terms = []
  1.upto(row_num){|d|
    1.upto(d){|row|
      terms << a[row][d - row]
    }
  }
  terms
end

row_num = 140
l = (ARGV[1] || 5).to_i
ary = antidiagonal_terms(row_num, l)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
