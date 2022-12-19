# Shelly

## Build
1. Clone the repository
2. `cd shelly`
3. `mkdir bulid && cd build`
4. `cmake ..`
5. `make`

## Configure
A config file **MUST** be created in *~/.config/shelly/config.conf*

It's structure is following:
```
shelly_server=https://<your-server>.shelly.cloud
shelly_key=<your-key>
psql_uri=postgresql://<postgre-uri>
devices=<plug-id>,<plug-ip>
```

Note that cache will be stored under *~/.cache/shelly*

## Database

You must create a database with this structure:

```sql
create table public.shelly
(
    id          integer generated always as identity,
    type        varchar(10)                                    not null,
    device_id   varchar(20)                                    not null,
    power       double precision                               not null,
    temperature double precision                               not null,
    total       integer                                        not null,
    time        timestamp default timezone('utc'::text, now()) not null,
    data        json
);

create index shelly_time_index
    on public.shelly (time desc);

create index shelly_type_index
    on public.shelly (type);
``` 

## Run
Run this command as frequently as you want to gather data, but note that
for sensors, data retrieval will only occur when new data exists and with
a minimal interval of 5 minutes.
