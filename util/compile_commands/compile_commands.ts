//generate compile_commands.json
import child_process from "child_process";
import { writeFileSync } from "fs";

const process = child_process.spawn("make");

let stdout: string[] = [];
let compile_commands: {
  directory: string;
  file: string;
  command: string;
}[] = [];

const go_back_one_directory = (dir: string) => {
  let parts = dir.split("/");
  parts.pop();
  let newPath = parts.join("/");

  return newPath;
};

const get_source_file = (command: string) => {
  console.log(command);
  const split = command.split("-c");
  if (!split[1]) return null;
  return split[1].trim().split(" ")[0];
};

const replace_current_file = () => {
  const compile_commands_json = JSON.stringify(compile_commands, null, 2);

  writeFileSync("compile_commands.json", compile_commands_json, "utf8");

  console.log(
    "\x1b[36m%s\x1b[0m",
    "Successfully generated compile_commands.json"
  );
};

const parse = () => {
  let current_directory = "";
  stdout.forEach((command) => {
    if (/make(\[[0-9]+])?: Entering directory '/g.test(command)) {
      current_directory = command.split("'")[1];
    } else if (/make(\[[0-9]+])?: Leaving directory '/g.test(command)) {
      current_directory = go_back_one_directory(command.split("'")[1]);
    } else if (/gcc/.test(command)) {
      const source = get_source_file(command);
      if (source)
        compile_commands.push({
          directory: current_directory,
          file: source,
          command,
        });
    }
  });

  replace_current_file();
};

const finalize = () => {
  stdout = stdout.filter(function (entry) {
    return entry.trim() != "";
  });

  parse();
};

process.stdout.on("data", (data) => {
  stdout = stdout.concat(String(data).split("\r\n"));
});

let err: string = "";

process.stderr.on("data", (data) => {
  err = String(data).split("\r\n").join("");
});

process.on("close", (code) => {
  if (code !== 0) {
    console.error(err);
    console.error(
      "\x1b[31m%s\x1b[0m. Error code: %d",
      "Make did not exit successfully!",
      code
    );
  } else {
    finalize();
  }
});
