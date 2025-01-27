.mode csv
.import d4.csv data
.mode column

create view CNF as
    select file, trim(state) as state, trim(mem) as mem, trim(time) as time
    from data
    where file like '%.cnf';

create view PRJ as
    select replace(file, '.prj', '') as file, trim(state) as state, trim(mem) as mem, trim(time) as time
    from data
    where file like '%.cnf.prj';

create view PRJP as
    select replace(file, '.prj.p', '') as file, trim(state) as state, trim(mem) as mem, trim(time) as time
    from data
    where file like '%.cnf.prj.p';

create view S0 as
    select replace(file, '.s0', '') as file, trim(state) as state, trim(mem) as mem, trim(time) as time
    from data
    where file like '%.cnf.s0';

create view S0P as
    select replace(file, '.s0.p', '') as file, trim(state) as state, trim(mem) as mem, trim(time) as time
    from data
    where file like '%.cnf.s0.p';

create view S1 as
    select replace(file, '.s1', '') as file, trim(state) as state, trim(mem) as mem, trim(time) as time
    from data
    where file like '%.cnf.s1';

create view S1P as
    select replace(file, '.s1.p', '') as file, trim(state) as state, trim(mem) as mem, trim(time) as time
    from data
    where file like '%.cnf.s1.p';

select count(*) as 'nb cnf'
from cnf
where state = 'done';

select count(*) as 'nb cnf err'
from cnf
where state like '%err%';

select count(*) as 'nb prjp + s0p + s1p'
from cnf
    join prjp on cnf.file = prjp.file
    join s0p on cnf.file = s0p.file
    join s1p on cnf.file = s1p.file
where prjp.state = 'done'
    and s0p.state = 'done'
    and s1p.state = 'done';

select count(*) as 'nb prj + s0 + s1'
from cnf
    join prj on cnf.file = prj.file
    join s0 on cnf.file = s0.file
    join s1 on cnf.file = s1.file
where prj.state = 'done'
    and s0.state = 'done'
    and s1.state = 'done';

-- .mode csv
-- select cnf.file
-- from cnf
--     join prjp on cnf.file = prjp.file
--     join s0p on cnf.file = s0p.file
--     join s1p on cnf.file = s1p.file
-- where prjp.state = 'done'
--     and s0p.state = 'done'
--     and s1p.state = 'done';
