# Triangle read by rows where T(n, k) is the number of strict
# integer partitions of 2*n with reverse-alternating sum 2*k.

def reverse_alternating_sum(partition)
  sum = 0
  partition.each_with_index{|value, index|
    sum += ((partition.length - index).odd? ? value : -value)
  }
  sum
end

def strict_partitions(total, max_part = total, partition = [], &block)
  if total == 0
    yield partition
    return
  end
  return if max_part <= 0

  upper = [max_part, total].min
  upper.downto(1){|part|
    strict_partitions(total - part, part - 1, partition + [part], &block)
  }
end

def t(n, k)
  count = 0
  strict_partitions(2 * n){|partition|
    count += 1 if reverse_alternating_sum(partition) == 2 * k
  }
  count
end

p ary = (0..20).map{|n| (0..n).map{|k| t(n, k)}}.flatten
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}