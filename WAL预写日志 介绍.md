#Write-Ahead Log (WAL预写日志) 介绍
WAL的主要意思是说在将元数据的变更操作写入到持久稳定的db之前，先预先写入到一个log中，然后再由另外的操作将log apply到外部的持久db里去。这种模式会减少掉每次的db写入操作，尤其当系统要处理大量的transaction操作的时候，WAL的方式相比较于实时同步db的方式有着更高的效率。

WAL还有一点很重要的帮助是可以在disaster recovery过程中起到状态恢复的作用，系统在load完元数据db后，再把未来得及提交的WAL apply进来，就能恢复成和之前最终一致的状态。

######WAL不记录元数据的本身，而是变更的record

MYSQL中也存在WAL机制
