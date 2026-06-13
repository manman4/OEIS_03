# A[row][col] corresponds to a(row, col-1, l).

def antidiagonal_terms(row_num, l = 3)
  k_limit = Array.new(row_num + 1, 0)
  1.upto(row_num) do |row|
    k_limit[row] = (row_num - row) * l
  end

  a = Array.new(row_num + 1) { |row| Array.new(k_limit[row] + 1, 0) }

  0.upto(k_limit[1]) do |col|
    a[1][col] = 1
  end

  2.upto(row_num) do |row|
    a[row][0] = 0
  end

  2.upto(row_num) do |row|
    1.upto(k_limit[row]) do |col|
      total = a[row][col - 1]
      1.upto(row - 1) do |j|
        total += a[j][col + l - 1] * a[row - j][col - 1]
      end
      a[row][col] = total
    end
  end

  terms = []
  1.upto(row_num) do |d|
    1.upto(d) do |row|
      terms << a[row][d - row]
    end
  end
  terms
end

row_num = 140
l = (ARGV[1] || 3).to_i
ary = antidiagonal_terms(row_num, l)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
