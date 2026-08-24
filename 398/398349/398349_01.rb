require 'prime'

def prime_sum?(x)
  x >= 2 && Prime.prime?(x)
end

def prime_set_partitions(n)
  full = (1 << n) - 1
  block_sum = Array.new(1 << n, 0)
  (0...(1 << n)).each do |mask|
    s = 0
    n.times { |i| s += i + 1 if mask[i] == 1 }
    block_sum[mask] = s
  end
  memo = {}
  f = lambda do |mask|
    return 1 if mask.zero?
    memo[mask] ||= begin
      low  = mask & -mask
      rest = mask ^ low
      total = 0
      sub = rest
      loop do
        block = low | sub
        total += f.call(mask ^ block) if prime_sum?(block_sum[block])
        break if sub.zero?
        sub = (sub - 1) & rest
      end
      total
    end
  end
  f.call(full)
end

p (1..14).map{|n| prime_set_partitions(n)}
