# a(n) = Sum_{k=0..n} A344650(k) * A000041(n-k).
# A000041(n) は n の整数分割数。
# A344650の結果はb344650.txtに保存されている。

a344650 = []
File.foreach(File.join(__dir__, "b344650.txt")){|line|
  line = line.strip
  next if line.empty?
  parts = line.split
  a344650[parts[0].to_i] = parts[1].to_i
}

max_n = 1000

def partition_numbers(max_n)
  ary = Array.new(max_n + 1, 0)
  ary[0] = 1
  (1..max_n).each{|part|
    (part..max_n).each{|sum|
      ary[sum] += ary[sum - part]
    }
  }
  ary
end

partitions = partition_numbers(max_n)

ary = (0..max_n).map{|n|
  (0..n).sum{|k| a344650[k] * partitions[n - k]}
}

(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
