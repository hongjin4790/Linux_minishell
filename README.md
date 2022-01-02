# Linux_minishell

- [구현한 명령어 목록](#I.-구현한-명령어-목록)
- [구현한 명령어에 대한 설명 및 사용 예제](#II.-구현한-명령어에-대한-설명-및-사용-예제)
- [주요 코드에 대한 코드 분석](#III)

## I. 구현한 명령어 목록

![image](https://user-images.githubusercontent.com/29851704/147875922-e11d5748-17a9-438c-88bb-80e1a740edf9.png)

## II. 구현한 명령어에 대한 설명 및 사용 예제
- cd: 디렉토리를 이동하는 명령어

![image](https://user-images.githubusercontent.com/29851704/147875952-ca62d80b-61f8-42ae-8e37-dda7fb2bb8d5.png)

- help: 어떤 명령어가 있는지 보여주는 명령어
- exit: 정상적으로 프로세스를 종료하는 명령어
- ls: 디렉토리 안의 파일과 디렉토리를 보여주는 명령어

![image](https://user-images.githubusercontent.com/29851704/147875977-1ca758d3-faa2-476c-81a9-3ab54d16a810.png)

- cat: 파일의 내용을 읽어 출력해주는 명령어

![image](https://user-images.githubusercontent.com/29851704/147875998-c57c335a-3d08-4021-89a0-60f632f41783.png)

- pwd: 현재 내 디렉토리 위치를 출력하는 명령어

![image](https://user-images.githubusercontent.com/29851704/147876011-f37d1855-030d-4a92-b978-ebdbf2bb349b.png)

- cp: 파일, 디렉토리를 복사하는 명령어

![image](https://user-images.githubusercontent.com/29851704/147876020-72970b40-34ca-4cfc-8ace-a46254990fef.png)

- stat: 디렉토리나 파일의 정보를 확인할 수 있는 명령어

![image](https://user-images.githubusercontent.com/29851704/147876028-75541d4a-81ec-4f1b-a61e-055e1abb5ed1.png)

- mkdir: 디렉토리를 생성하는 명령어

![image](https://user-images.githubusercontent.com/29851704/147876058-c74de3d4-18c6-4eaf-8746-4eb77c47092b.png)

- ln: 링크파일을 만드는 명령어

![image](https://user-images.githubusercontent.com/29851704/147876070-1412a032-e93d-4006-93f1-0e3a9f17d9af.png)

- chmod: 파일의 권한을 변경할 수 있게 하는 명령어

![image](https://user-images.githubusercontent.com/29851704/147876074-b7c83150-01b8-45a0-8e9e-aa3a3e91011f.png)

- ps: 현재 실행중인 프로세스 목록과 상태를 보여주는 명령어

![image](https://user-images.githubusercontent.com/29851704/147876094-a5f1923d-2e52-4cd3-9381-85b44293f3da.png)

- alias: 원래의 명령어를 원하는 별명으로 바꾸는 명령어

![image](https://user-images.githubusercontent.com/29851704/147876106-90cb951c-76e8-4907-aba4-813811df7e5a.png)

- rm: 파일,디렉토리를 삭제할 수 있는 명령어

![image](https://user-images.githubusercontent.com/29851704/147876110-ed60a8ff-c5df-4ad7-a6ba-f59005154029.png)

## III. 주요 코드에 대한 코드 분석
### msh_read_line() 함수

![image](https://user-images.githubusercontent.com/29851704/147876128-abb8443a-1cee-40e0-86cd-b7d00ffd328e.png)

- buffer생성하고 입력한 내용을 버퍼에 넣고 EOF 또는 \n이면 버퍼 리턴
- 버퍼크기보다 많으면 realloc으로 크기 증가  << 예외처리

### msh_split_line() 함수

![image](https://user-images.githubusercontent.com/29851704/147876164-75274fd9-bbb8-4c9b-a2d5-d2c165320be1.png)

- strtok를 사용해서 msh_read_line()에서 얻은 line값을 분리해서 tokens에 token이 NULL일 될 때까지 반복한고 넣는다.
- 전역변수로 ar을 선언하고 ar은 argc와같은 역할을 한다.

### alias() 함수

![image](https://user-images.githubusercontent.com/29851704/147876232-89d10ad2-c00f-4654-84c3-ad97c33d6e63.png)

- eliminate함수를 써서 ‘를 제거해준다. 그리고 =을 기준으로 sl 과 ls로 분리하고 구조체에 명령어 인덱스, 바꾼이름, 원래이름을 저장한다. 그리고 명령어가 저장되어있는 함수에 추가한다.

### msh_cd() 함수

![image](https://user-images.githubusercontent.com/29851704/147876245-e13e74d9-69db-4631-9ae6-54132de28472.png)

- chdir을 사용해서 디렉토리를 이동한다.

### msh_ls() 함수

![image](https://user-images.githubusercontent.com/29851704/147876262-601c1f71-34be-4e51-907d-10d8960da19f.png)

- 디렉토리의 처음부터 파일이나 디렉토리를 순서대로 읽는다.
- buf에 읽어온 파일의 이름을 저장한다.
- buf.st_mode가 정규파일인지, 디렉토리인지 확인하고 마지막에 이름 출력

### msh_pwd() 함수

![image](https://user-images.githubusercontent.com/29851704/147876268-fb2306d1-c61a-4e3e-b279-b9c3bd0e5175.png)

- getcwd를 사용하면 현재 디렉토리를 얻을 수 있다.

### msh_cat() 함수

![image](https://user-images.githubusercontent.com/29851704/147876276-78ccf504-d9a7-4f7b-bf92-8851fdf04fad.png)

- fgets를 사용해서 한줄씩 읽어온다.

### msh_cp() 함수

![image](https://user-images.githubusercontent.com/29851704/147876287-7e138f69-1980-443a-8de1-ffd3d70273fa.png)

- fdin 읽기만가능하게 args[1]파일을 연다.
- O_WRONLY: 읽기 쓰기 가능
- O_CREAT 파일이없으면 생성
- O_EXCL: O_CREAT 옵션과 함께 사용할 경우 기존에 없는 파일이면 파일을 생성하지만, 이미 있으면 파일을 생성하지 않고 오류 메시지를 출력

### msh_stat() 함수

![image](https://user-images.githubusercontent.com/29851704/147876294-ecb784a3-e5c3-4e33-b716-dfb33702a92c.png)

- sb에 args[1] 파일의 구조체 저장
- sb.st_dev 파일의 이름과 inode를 담은 장치번호
- sb.st_mode: 파일의 종류 및 접근권한
- sb.st_ino: inode 번호
- sb.st_nlink: hardlink된 횟수
- sb.st_gid: GID
- sb.st_blksize: 블록사이즈
- sb.st_size:파일의 크기
- sb.st_blocks: 할당된 블록의 개수 

### msh_chmod() 함수

![image](https://user-images.githubusercontent.com/29851704/147876309-4de05d81-08f5-4693-8075-da579ff57066.png)


- 소유자: S_IRUSR, S_IWUSR, S_IXUSR
- 그룹: S_IRGRP ,S_IWGRP, S_IXGRP
- 그 외 사용자 : S_IROTH, S_IWOTH, S_IXOTH 

### msh_ps() 함수

![image](https://user-images.githubusercontent.com/29851704/147876316-dab592f1-eb01-4c2e-a9d6-3a5f27a984a8.png)

- 내 터미널을 /proc/self에서 찾고 /proc에 있는 디렉토리를 처음부터 찾으면서 나와 터미널이 같으면 출력하도록 한다. 
- command는 /proc/pid번호/cmdline에 저장된다.

